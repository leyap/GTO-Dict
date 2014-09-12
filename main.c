#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chs.h"


typedef struct {
	int offset;
	int leng;
}Stable;

long table_list_leng;

GtkWidget *view;
GtkTextBuffer *text_buffer;
//GtkTextIter *Iter;
GtkTextIter start, end;

GtkWidget *entry_word;

gchar *word4_buffer;
Stable *table;

int my_strcmp (char *ch, char *chs)
{
	for (; *ch == *chs; ch++, chs++);
	if (*ch == '\0')
		return 0;
	else if (*ch > *chs)
		return 1;
	else
		return -1;
}

//符合条件1则返回匹配度，否则返回0
int my_strcmp1 (char *ch, char *chs)
{
	int l;
	for (l=0; *ch == *chs; ch++,chs++,l++);	//后来加的分号
	//if (l > 3 || *ch == '\0' || (*chs == '#' && l > 2)) {
	if (*ch == '\0' || (*chs == '#' && l > 2))
		return l;
	return 0;
}


//符合条件2则返回匹配度，否则返回0,(已不用)
int my_strcmp2 (char *ch, char *chs)
{
	int l;
	for (l=0; *ch == *chs; ch++,chs++,l++);	//后来加的分号
	if (l > 3 || *ch == '\0' || (*chs == '#' && l > 2))
		return l;
	return 0;
}

//获取字符串匹配度
int get_cmpval (char *ch, char *chs)
{
	int l;
	for (l=0; *ch == *chs; ch++,chs++,l++);
	return l;
}

//从头查寻
void search1 (Stable *table, int leng, gchar *ch, gchar *word4_buffer)
{
	int i;
	int cmpval;
	for (i = 0; i < leng;i++ ) {
		cmpval = my_strcmp1 (ch, word4_buffer + (table + i) -> offset);
		if (cmpval) {
			gtk_text_buffer_set_text (GTK_TEXT_BUFFER (text_buffer), word4_buffer + (table + i) -> offset, (table + i) -> leng);
			for (i++; i<leng && get_cmpval (ch, word4_buffer + (table + i) -> offset) >= cmpval; i++) {
				gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (text_buffer), &start, &end);
				gtk_text_buffer_insert (GTK_TEXT_BUFFER (text_buffer), &end, word4_buffer + (table + i)->offset, (table + i) -> leng);
			}
			break;
		}
	}
}

//截断查寻
void search2 (Stable *table, int leng, gchar *ch, gchar *word4_buffer)
{
	int i;
	int cmpval;
	for (i = 0; i < leng; i++) {
		cmpval = my_strcmp2 (ch, word4_buffer + (table + i) -> offset);
		if (cmpval) {
			gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (text_buffer), &start, &end);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (text_buffer), &end,"\n", 1);
			for (; i<leng && get_cmpval (ch, word4_buffer + (table + i) -> offset) >= cmpval; i++) {
				gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (text_buffer), &start, &end);
				gtk_text_buffer_insert (GTK_TEXT_BUFFER (text_buffer), &end, word4_buffer + (table + i) -> offset, (table + i) -> leng);
			}
			break;
		}
	}
}

/*
   int my_bsearch (Stable *table, int leng, gchar *ch, gchar *word4_buffer)
   {
   int left, right, mid;
   int cmp;

   for (left=0,right=leng-1; left<=right; ) {
   mid = (left + right) / 2;
   cmp = my_strcmp (ch, word4_buffer + (table + mid) -> offset);
   if (cmp > 0) {
   left = mid + 1;
   continue;
   }
   if (cmp < 0) {
   right = mid - 1;
   continue;
   }
   return mid;
   }
   return -1;
   }
   */


void closeApp (GtkWidget *window, gpointer data)
{
	gtk_main_quit ();
}

gboolean delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	printf ("In delete_event\n");
	//GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	//gtk_widget_show_all (window);
	return FALSE;
}

void find_func (GtkWidget *window, gpointer data)
{
	G_CONST_RETURN gchar *my_word;
	char *cp_word = NULL;
	int ch_leng;
	int i;

	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (text_buffer), "", 0);

	my_word = (gchar *)gtk_entry_get_text (GTK_ENTRY (entry_word));

	if (*my_word) {
		cp_word = strcopy ((char *)my_word);
		del_space (cp_word);
		my_to_lower (cp_word);
		ch_leng = strlen (cp_word);
		printf ("=%2d : %s\n",ch_leng, cp_word);
		search1 (table, table_list_leng, (gchar *)(cp_word), word4_buffer);
		for (i=1; i < ch_leng-2; i++)
			search2 (table, table_list_leng, (gchar *)(cp_word + i), word4_buffer);
	}
}

// get file length
long get_file_leng (FILE *fp)
{
	long leng;
	fseek (fp, 0L, SEEK_END);
	leng = ftell (fp);
	rewind (fp);
	return leng;
}


int main (int argc, char *argv[])
{
	GtkWidget *window;
	//GtkWidget *button_find;
	GtkWidget *scrolled;
	GtkWidget *hbox;
	GtkWidget *vbox;

	FILE *word4_fp, *word4_list_fp;
	char *word_name = "word4.txt";
	char *list_name = "word4.list";
	long word_file_leng;
	long table_file_leng;



	if ((word4_fp = fopen (word_name, "rb")) == NULL) {
		printf ("can't open file %s\n", word_name);
		exit (0);
	}
	if ((word4_list_fp = fopen (list_name, "rb")) ==NULL) {
		printf ("can't open file %s\n", list_name);
		exit (0);
	}

	word_file_leng = get_file_leng (word4_fp);
	table_file_leng = get_file_leng (word4_list_fp);
	table_list_leng = table_file_leng / sizeof (Stable);

	printf ("word = %d\n", (int)word_file_leng);
	printf ("table = %d\n", (int)table_list_leng);

	word4_buffer = (char *)malloc (word_file_leng);
	table = (Stable *)malloc (table_file_leng);

	fread (word4_buffer, word_file_leng, 1, word4_fp);
	fread (table, table_file_leng, 1, word4_list_fp);

	gtk_init (&argc, &argv);

	window	=	gtk_window_new (GTK_WINDOW_TOPLEVEL);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	//hbox	=	gtk_hbox_new (FALSE, 2);;
	//vbox	=	gtk_vbox_new (FALSE, 1);
	view	=	gtk_text_view_new ();
	text_buffer	=	gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	//button_find	=	gtk_button_new_with_label ("查找");
	entry_word	=	gtk_entry_new ();
	scrolled	=	gtk_scrolled_window_new (NULL,NULL);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled),view);

	gtk_window_set_title		(GTK_WINDOW (window), "大学英语四级词汇 0.04");
	gtk_window_set_position		(GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size	(GTK_WINDOW (window), 380,436);
	gtk_text_buffer_set_text(text_buffer, "\
			\n\n\n\n  			英语詞典！\
			\n\n  			支持自动模糊连续查询\
			\n\n  			词库可视可修改\
			\n\n  			作者：lisper <lisper.li@dfrobot.com>\
			\n\n  			2012-10-07于阜阳老家", -1);

	g_signal_connect (window, "destroy", G_CALLBACK (closeApp), NULL);
	g_signal_connect (window, "delete_event", G_CALLBACK (delete_event), NULL);
	// g_signal_connect (button_find, "clicked", G_CALLBACK (find_func), NULL);
	g_signal_connect (entry_word, "activate", G_CALLBACK (find_func), NULL);
	g_signal_connect (entry_word, "changed", G_CALLBACK (find_func), NULL);

	gtk_box_pack_start	(GTK_BOX (hbox), entry_word, TRUE, TRUE, 2);
	//gtk_box_pack_start (GTK_BOX (hbox), button_find, FALSE, FALSE, 2);
	gtk_box_pack_start	(GTK_BOX (vbox), hbox, FALSE, FALSE, 2);
	gtk_box_pack_start	(GTK_BOX (vbox), scrolled, TRUE, TRUE, 2);
	gtk_container_add	(GTK_CONTAINER (window), vbox);

	gtk_widget_show_all (window);
	gtk_main ();

	return 0;
}

