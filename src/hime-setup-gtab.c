/* Copyright (C) 2011 Edward Der-Hua Liu, Hsin-Chu, Taiwan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "hime.h"
#include "gtab.h"


static GtkWidget *check_button_gtab_dup_select_bell,
                 *opt_gtab_press_full_auto_send,
                 *opt_gtab_pre_select,
                 *opt_gtab_disp_partial_match,
                 *check_button_gtab_disp_key_codes,
                 *check_button_gtab_disp_im_name,
                 *check_button_gtab_invalid_key_in,
                 *check_button_gtab_shift_phrase_key,
                 *check_button_gtab_hide_row2,
                 *check_button_gtab_in_row1,
                 *opt_gtab_vertical_select,
                 *opt_gtab_unique_auto_send,
                 *check_button_gtab_que_wild_card,
                 *check_button_gtab_que_wild_card_asterisk,
                 *check_button_gtab_pho_query,
                 *check_button_gtab_phrase_pre_select;

extern GtkWidget *check_button_hime_capslock_lower;
extern gboolean button_order;

struct {
  unich_t *str;
  int num;
} spc_opts[] = {
  {N_("由 .gtab 指定"), GTAB_space_auto_first_none},
  {N_("按空白立即送出第一字(嘸蝦米、大易)"), GTAB_space_auto_first_any},
  {N_("按滿按空白送出第一字"), GTAB_space_auto_first_full},
  {N_("按滿按空白不送出第一字(倉頡, 行列)"), GTAB_space_auto_first_nofull},
  { NULL, 0},
};


struct {
  unich_t *str;
  int num;
} auto_select_by_phrase_opts[] = {
  {N_("由.gtab指定"), GTAB_OPTION_AUTO},
  {N_("全部開啟"), GTAB_OPTION_YES},
  {N_("全部關閉"), GTAB_OPTION_NO},
  { NULL, 0},
};

void save_tsin_eng_pho_key();
static GtkWidget *hime_gtab_conf_window;
static GtkWidget *opt_spc_opts, *opt_auto_select_by_phrase;

void save_menu_val(char *config, GtkWidget *opt)
{
  int idx = gtk_combo_box_get_active (GTK_COMBO_BOX (opt));
  save_hime_conf_int(config, auto_select_by_phrase_opts[idx].num);
}

void save_gtab_conf()
{
  save_tsin_eng_pho_key();
  save_hime_conf_int(GTAB_DUP_SELECT_BELL,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_dup_select_bell)));

  save_menu_val(GTAB_PRE_SELECT, opt_gtab_pre_select);

  save_menu_val(GTAB_DISP_PARTIAL_MATCH, opt_gtab_disp_partial_match);

  save_hime_conf_int(GTAB_DISP_KEY_CODES,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_disp_key_codes)));

  save_hime_conf_int(GTAB_DISP_IM_NAME,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_disp_im_name)));

  save_hime_conf_int(GTAB_INVALID_KEY_IN,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_invalid_key_in)));

  save_hime_conf_int(GTAB_SHIFT_PHRASE_KEY,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_shift_phrase_key)));

  save_hime_conf_int(GTAB_HIDE_ROW2,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_hide_row2)));

  save_hime_conf_int(GTAB_IN_ROW1,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_in_row1)));

  save_menu_val(GTAB_VERTICAL_SELECT, opt_gtab_vertical_select);

  save_hime_conf_int(GTAB_QUE_WILD_CARD,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_que_wild_card)));

  save_hime_conf_int(GTAB_QUE_WILD_CARD_ASTERISK,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_que_wild_card_asterisk)));

  save_hime_conf_int(GTAB_PHO_QUERY,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_pho_query)));

  save_hime_conf_int(HIME_CAPSLOCK_LOWER,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_hime_capslock_lower)));

  save_hime_conf_int(GTAB_PHRASE_PRE_SELECT,
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_gtab_phrase_pre_select)));

  int idx = gtk_combo_box_get_active (GTK_COMBO_BOX (opt_spc_opts));
  save_hime_conf_int(GTAB_SPACE_AUTO_FIRST, spc_opts[idx].num);


  save_menu_val(GTAB_AUTO_SELECT_BY_PHRASE, opt_auto_select_by_phrase);

  save_menu_val(GTAB_PRESS_FULL_AUTO_SEND, opt_gtab_press_full_auto_send);

  save_menu_val(GTAB_UNIQUE_AUTO_SEND, opt_gtab_unique_auto_send);

  send_hime_message(GDK_DISPLAY(), CHANGE_FONT_SIZE);
}

void destroy_gtab_window()
{
  gtk_widget_destroy(hime_gtab_conf_window); hime_gtab_conf_window = NULL;
}

extern char utf8_edit[];
static gboolean cb_gtab_edit_append( GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer   data )
{
  load_gtab_list(FALSE);
  char *fname = inmd[default_input_method].filename;
  if (!fname)
    return TRUE;

  char append_fname[128];
  sprintf(append_fname, "~/.config/hime/%s.append", fname);

  char prepare[128];
  sprintf(prepare, HIME_SCRIPT_DIR"/gtab.append_prepare %s", append_fname);
  system(prepare);

  char exec[128];

  sprintf(exec, "%s %s", utf8_edit, append_fname);
  dbg("exec %s\n", exec);
  system(exec);
  return TRUE;
}

static GtkWidget *create_spc_opts()
{
  GtkWidget *frame = gtk_frame_new(_("空白鍵選項"));

  opt_spc_opts = gtk_combo_box_new_text ();
  gtk_container_add (GTK_CONTAINER (frame), opt_spc_opts);

  int i, current_idx=0;

  for(i=0; spc_opts[i].str; i++) {
    if (spc_opts[i].num == gtab_space_auto_first)
      current_idx = i;
    gtk_combo_box_append_text (GTK_COMBO_BOX_TEXT (opt_spc_opts), _(spc_opts[i].str));
  }

  gtk_combo_box_set_active (GTK_COMBO_BOX (opt_spc_opts), current_idx);

  return frame;
}

static GtkWidget *create_auto_select_by_phrase_opts(GtkWidget **out, int val)
{
  *out = gtk_combo_box_new_text ();

  int i, current_idx=0;

  for(i=0; auto_select_by_phrase_opts[i].str; i++) {
    if (auto_select_by_phrase_opts[i].num == val)
      current_idx = i;
    gtk_combo_box_append_text (GTK_COMBO_BOX_TEXT (*out), _(auto_select_by_phrase_opts[i].str));
  }

  gtk_combo_box_set_active (GTK_COMBO_BOX (*out), current_idx);

  return *out;
}


GtkWidget *create_en_pho_key_sel(char *s);

GtkWidget *create_gtab_widget (gsize type);

#ifdef USE_TABS
GtkWidget *create_gtab_window ()
{
  hime_gtab_conf_window = create_gtab_widget(0);
  return hime_gtab_conf_window;
}
#else
static gboolean cb_gtab_conf_ok( GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer   data )
{
  save_gtab_conf();
  destroy_gtab_window();
  return TRUE;
}

static gboolean close_gtab_conf_window( GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer   data )
{
  destroy_gtab_window();
  return TRUE;
}

void create_gtab_conf_window(GtkWidget *widget, gsize type)
{
  if (hime_gtab_conf_window) {
    gtk_window_present(GTK_WINDOW(hime_gtab_conf_window));
    return;
  }

  hime_gtab_conf_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  if (hime_setup_window_type_utility)
    gtk_window_set_type_hint(GTK_WINDOW(hime_gtab_conf_window), GDK_WINDOW_TYPE_HINT_UTILITY);
  gtk_window_set_position(GTK_WINDOW(hime_gtab_conf_window), GTK_WIN_POS_MOUSE);
  gtk_window_set_has_resize_grip(GTK_WINDOW(hime_gtab_conf_window), FALSE);

  g_signal_connect (G_OBJECT (hime_gtab_conf_window), "delete_event",
                    G_CALLBACK (close_gtab_conf_window),
                    NULL);

  gtk_window_set_title (GTK_WINDOW (hime_gtab_conf_window), _("倉頡/行列/大易設定"));
  gtk_container_set_border_width (GTK_CONTAINER (hime_gtab_conf_window), 3);

  GtkWidget *top_widget = create_gtab_widget (type);
  gtk_container_add (GTK_CONTAINER (hime_gtab_conf_window), top_widget);

  GtkWidget *hbox_cancel_ok = gtk_hbox_new (FALSE, 10);
  gtk_grid_set_column_homogeneous(GTK_GRID(hbox_cancel_ok), TRUE);
  gtk_box_pack_start (GTK_BOX (top_widget), hbox_cancel_ok, FALSE, FALSE, 0);

  GtkWidget *button_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  if (button_order)
    gtk_box_pack_end (GTK_BOX (hbox_cancel_ok), button_cancel, TRUE, TRUE, 0);
  else
    gtk_box_pack_start (GTK_BOX (hbox_cancel_ok), button_cancel, TRUE, TRUE, 0);

  g_signal_connect (G_OBJECT (button_cancel), "clicked",
                            G_CALLBACK (close_gtab_conf_window),
                            G_OBJECT (hime_gtab_conf_window));

  GtkWidget *button_ok = gtk_button_new_from_stock (GTK_STOCK_OK);
#if !GTK_CHECK_VERSION(2,91,2)
  if (button_order)
    gtk_box_pack_end (GTK_BOX (hbox_cancel_ok), button_ok, TRUE, TRUE, 0);
  else
    gtk_box_pack_start (GTK_BOX (hbox_cancel_ok), button_ok, TRUE, TRUE, 0);
#else
  if (button_order)
    gtk_grid_attach_next_to (GTK_BOX (hbox_cancel_ok), button_ok, button_cancel, GTK_POS_LEFT, 1, 1);
  else
    gtk_grid_attach_next_to (GTK_BOX (hbox_cancel_ok), button_ok, button_cancel, GTK_POS_RIGHT, 1, 1);
#endif

  g_signal_connect_swapped (G_OBJECT (button_ok), "clicked",
                            G_CALLBACK (cb_gtab_conf_ok),
                            G_OBJECT (hime_gtab_conf_window));

  GTK_WIDGET_SET_FLAGS (button_ok, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button_ok);

  gtk_widget_show_all (hime_gtab_conf_window);

  return;
}

#endif

GtkWidget *create_gtab_widget (gsize type)
{
  GtkWidget *top_widget = NULL;

  load_settings();

  GtkWidget *vbox_top = gtk_vbox_new (FALSE, 10);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_top), GTK_ORIENTATION_VERTICAL);
  top_widget = vbox_top;

#ifdef USE_WIDE
  GtkWidget *hbox_lr = gtk_vbox_new (FALSE, 10);
#else
  GtkWidget *hbox_lr = gtk_hbox_new (FALSE, 10);
#endif
  gtk_box_pack_start (GTK_BOX (vbox_top), hbox_lr, FALSE, FALSE, 0);

#if !USE_WIDE
  GtkWidget *frame_gtab_l = gtk_frame_new(_("外觀"));
  gtk_container_set_border_width (GTK_CONTAINER (frame_gtab_l), 10);
  gtk_box_pack_start (GTK_BOX (hbox_lr), frame_gtab_l, TRUE, TRUE, 0);
#endif
  GtkWidget *vbox_gtab_l = gtk_vbox_new (FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_l), GTK_ORIENTATION_VERTICAL);
#ifdef USE_WIDE
  gtk_box_pack_start (GTK_BOX (hbox_lr), vbox_gtab_l, TRUE, TRUE, 0);
  if (type==2) gtk_widget_set_no_show_all(vbox_gtab_l, TRUE);
  GtkWidget *box = vbox_gtab_l;
  GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
  vbox_gtab_l= gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_gtab_l), 10);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_l), GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_start (GTK_BOX (hbox), vbox_gtab_l, FALSE, FALSE, 0);
#else
  gtk_container_add (GTK_CONTAINER (frame_gtab_l), vbox_gtab_l);
#endif

#if !USE_WIDE
  GtkWidget *frame_gtab_r = gtk_frame_new(_("行為"));
  gtk_container_set_border_width (GTK_CONTAINER (frame_gtab_r), 10);
  gtk_box_pack_start (GTK_BOX (hbox_lr), frame_gtab_r, TRUE, TRUE, 0);
#endif
  GtkWidget *vbox_gtab_r = gtk_vbox_new (FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_r), GTK_ORIENTATION_VERTICAL);

#ifdef USE_WIDE
  gtk_box_pack_start (GTK_BOX (hbox_lr), vbox_gtab_r, TRUE, TRUE, 0);
  if (type==1) gtk_widget_set_no_show_all(vbox_gtab_r, TRUE);
#else
  gtk_container_add (GTK_CONTAINER (frame_gtab_r), vbox_gtab_r);
#endif

#define SPC 1


  GtkWidget *hbox_gtab_pre_select = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_pre_select, FALSE, FALSE, 0);
  opt_gtab_pre_select = gtk_label_new (_("預覽/預選 字"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_pre_select),opt_gtab_pre_select,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_pre_select), create_auto_select_by_phrase_opts(&opt_gtab_pre_select, gtab_pre_select),  FALSE, FALSE, 0);

  GtkWidget *hbox_gtab_disp_partial_match = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_disp_partial_match, FALSE, FALSE, 0);
  opt_gtab_disp_partial_match = gtk_label_new (_("預選列中顯示部份符合的字"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_disp_partial_match), opt_gtab_disp_partial_match,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_disp_partial_match), create_auto_select_by_phrase_opts(&opt_gtab_disp_partial_match, gtab_disp_partial_match), FALSE, FALSE, 0);

  GtkWidget *hbox_gtab_vertical_select = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_vertical_select, FALSE, FALSE, 0);
  GtkWidget *label_gtab_vertical_select = gtk_label_new (_("垂直選擇"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_vertical_select), label_gtab_vertical_select,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_vertical_select), create_auto_select_by_phrase_opts(&opt_gtab_vertical_select, gtab_vertical_select),  FALSE, FALSE, 0);


#ifdef USE_WIDE
  vbox_gtab_l= gtk_vbox_new (FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_l), GTK_ORIENTATION_VERTICAL);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_gtab_l), 10);
  gtk_box_pack_start (GTK_BOX (hbox), vbox_gtab_l, FALSE, FALSE, 0);
#endif

  GtkWidget *hbox_gtab_disp_key_codes = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_disp_key_codes, FALSE, FALSE, 0);
  check_button_gtab_disp_key_codes = gtk_check_button_new_with_label (_("顯示拆碼"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_disp_key_codes), check_button_gtab_disp_key_codes,  FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_disp_key_codes),
     gtab_disp_key_codes);
  GtkWidget *hbox_gtab_disp_im_name = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_disp_im_name, FALSE, FALSE, 0);
  check_button_gtab_disp_im_name = gtk_check_button_new_with_label (_("顯示輸入法名稱"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_disp_im_name), check_button_gtab_disp_im_name,  FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_disp_im_name),
     gtab_disp_im_name);

  GtkWidget *hbox_gtab_hide_row2 = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_hide_row2, FALSE, FALSE, 0);
  check_button_gtab_hide_row2 = gtk_check_button_new_with_label (_("隱藏第二列 (輸入字根…)"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_hide_row2), check_button_gtab_hide_row2,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_hide_row2),
     gtab_hide_row2);


  GtkWidget *hbox_gtab_in_row1 = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_l), hbox_gtab_in_row1, FALSE, FALSE, 0);
  check_button_gtab_in_row1 = gtk_check_button_new_with_label (_("將輸入字根移至第一列"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_in_row1), check_button_gtab_in_row1,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_in_row1),
     gtab_in_row1);

#ifdef USE_WIDE
  box = vbox_gtab_r;
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
  vbox_gtab_r= gtk_vbox_new (FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_r), GTK_ORIENTATION_VERTICAL);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_gtab_r), 10);
  gtk_box_pack_start (GTK_BOX (hbox), vbox_gtab_r, FALSE, FALSE, 0);
#endif

  GtkWidget *hbox_gtab_press_full_auto_send = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_press_full_auto_send, FALSE, FALSE, 0);
  GtkWidget *label_gtab_gtab_press_full_auto_send = gtk_label_new(_("按滿字根自動送字"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_press_full_auto_send), label_gtab_gtab_press_full_auto_send, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_press_full_auto_send), create_auto_select_by_phrase_opts(&opt_gtab_press_full_auto_send, gtab_press_full_auto_send),  FALSE, FALSE, 0);

  GtkWidget *hbox_gtab_auto_select_by_phrase = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_auto_select_by_phrase, FALSE, FALSE, 0);
  GtkWidget *label_gtab_auto_select = gtk_label_new(_("由詞庫自動選字"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_auto_select_by_phrase), label_gtab_auto_select,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_auto_select_by_phrase), create_auto_select_by_phrase_opts(&opt_auto_select_by_phrase, gtab_auto_select_by_phrase),  FALSE, FALSE, 0);
  check_button_gtab_phrase_pre_select = gtk_check_button_new_with_label (_("使用預選詞"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_auto_select_by_phrase), check_button_gtab_phrase_pre_select,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_phrase_pre_select), gtab_phrase_pre_select);


  GtkWidget *hbox_gtab_dup_select_bell = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_dup_select_bell, FALSE, FALSE, 0);
  check_button_gtab_dup_select_bell = gtk_check_button_new_with_label (_("有重複字時，發出嗶聲"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_dup_select_bell),check_button_gtab_dup_select_bell,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_dup_select_bell),
     gtab_dup_select_bell);

  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), create_spc_opts(), FALSE, FALSE, 0);

  GtkWidget *hbox_gtab_invalid_key_in = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_invalid_key_in, FALSE, FALSE, 0);
  check_button_gtab_invalid_key_in = gtk_check_button_new_with_label (_("可鍵入錯誤字根 (傳統)"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_invalid_key_in), check_button_gtab_invalid_key_in,  FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_invalid_key_in),
     gtab_invalid_key_in);


  GtkWidget *hbox_gtab_shift_phrase_key = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_shift_phrase_key, FALSE, FALSE, 0);
  check_button_gtab_shift_phrase_key = gtk_check_button_new_with_label (_("可用 Shift 輸入片語 (預設為 Alt-Shift)"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_shift_phrase_key), check_button_gtab_shift_phrase_key,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_shift_phrase_key),
     gtab_shift_phrase_key);

#ifdef USE_WIDE
  vbox_gtab_r= gtk_vbox_new (FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vbox_gtab_r), GTK_ORIENTATION_VERTICAL);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_gtab_r), 10);
  gtk_box_pack_start (GTK_BOX (hbox), vbox_gtab_r, FALSE, FALSE, 0);
#endif

  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), create_en_pho_key_sel(_("切換[中/英]輸入")), FALSE, FALSE, 0);

#if 0
  GtkWidget *hbox_hime_capslock_lower = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_hime_capslock_lower, FALSE, FALSE, 0);
  check_button_hime_capslock_lower = gtk_check_button_new_with_label (_("\t用小寫字母"));
  gtk_box_pack_start (GTK_BOX (hbox_hime_capslock_lower), check_button_hime_capslock_lower,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_hime_capslock_lower),
     hime_capslock_lower);
#endif

  GtkWidget *hbox_gtab_unique_auto_send = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_unique_auto_send, FALSE, FALSE, 0);
  GtkWidget *label_gtab_unique_auto_send = gtk_label_new (_("唯一選擇時自動送出"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_unique_auto_send), label_gtab_unique_auto_send,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_gtab_unique_auto_send),  create_auto_select_by_phrase_opts(&opt_gtab_unique_auto_send, gtab_unique_auto_send),  FALSE, FALSE, 0);

  GtkWidget *hbox_gtab_que_wild_card = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_que_wild_card, FALSE, FALSE, 0);
  check_button_gtab_que_wild_card = gtk_check_button_new_with_label (_("使用？萬用字元"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_que_wild_card), check_button_gtab_que_wild_card,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_que_wild_card),
     gtab_que_wild_card);

  GtkWidget *hbox_gtab_que_wild_card_asterisk = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_que_wild_card_asterisk, FALSE, FALSE, 0);
  check_button_gtab_que_wild_card_asterisk = gtk_check_button_new_with_label (_("使用＊萬用字元"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_que_wild_card_asterisk), check_button_gtab_que_wild_card_asterisk,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_que_wild_card_asterisk),
     gtab_que_wild_card_asterisk);

  GtkWidget *hbox_gtab_pho_query = gtk_hbox_new (FALSE, SPC);
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), hbox_gtab_pho_query, FALSE, FALSE, 0);
  check_button_gtab_pho_query = gtk_check_button_new_with_label (_("使用` 查詢同音字"));
  gtk_box_pack_start (GTK_BOX (hbox_gtab_pho_query), check_button_gtab_pho_query,  FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_gtab_pho_query),
     gtab_pho_query);

  GtkWidget *button_edit_append = gtk_button_new_with_label(_("編輯預設輸入法的使用者外加字詞"));
  gtk_box_pack_start (GTK_BOX (vbox_gtab_r), button_edit_append, FALSE, FALSE, 0);

  g_signal_connect_swapped (G_OBJECT (button_edit_append), "clicked",
                            G_CALLBACK (cb_gtab_edit_append), NULL);

  return top_widget;
}
