#ifndef GNOME_FUN_H
#define GNOME_FUN_H

#include <cstddef>

/* Copy-paste of necessary prototypes and definitions from gtk/gtk.h
   So, to build this code GTK itself is not needed. */

#ifndef __GTK_H__
typedef char gchar;
typedef short gshort;
typedef long glong;
typedef int gint;
typedef gint gboolean;
typedef unsigned char guchar;
typedef unsigned short gushort;
typedef unsigned long gulong;
typedef unsigned int guint;
typedef float gfloat;
typedef double gdouble;

typedef void *gpointer;
typedef const void *gconstpointer;
typedef gboolean (*GSourceFunc)(gpointer user_data);

typedef size_t GType;
typedef struct _GTypeInstance GTypeInstance;

typedef struct _GtkWindow GtkWindow;
typedef struct _GtkDialog GtkDialog;
typedef struct _GtkWidget GtkWidget;
typedef struct _GtkButton GtkButton;

#ifndef	FALSE
#define	FALSE (0)
#endif
#ifndef	TRUE
#define	TRUE (!FALSE)
#endif

typedef enum
{
	GTK_MESSAGE_INFO,
	GTK_MESSAGE_WARNING,
	GTK_MESSAGE_QUESTION,
	GTK_MESSAGE_ERROR,
	GTK_MESSAGE_OTHER
} GtkMessageType;

typedef enum
{
	GTK_DIALOG_MODAL               = 1 << 0,
	GTK_DIALOG_DESTROY_WITH_PARENT = 1 << 1,
	GTK_DIALOG_USE_HEADER_BAR      = 1 << 2
} GtkDialogFlags;

typedef enum
{
	GTK_RESPONSE_NONE         = -1,
	GTK_RESPONSE_REJECT       = -2,
	GTK_RESPONSE_ACCEPT       = -3,
	GTK_RESPONSE_DELETE_EVENT = -4,
	GTK_RESPONSE_OK           = -5,
	GTK_RESPONSE_CANCEL       = -6,
	GTK_RESPONSE_CLOSE        = -7,
	GTK_RESPONSE_YES          = -8,
	GTK_RESPONSE_NO           = -9,
	GTK_RESPONSE_APPLY        = -10,
	GTK_RESPONSE_HELP         = -11
} GtkResponseType;

typedef enum
{
	GTK_BUTTONS_NONE,
	GTK_BUTTONS_OK,
	GTK_BUTTONS_CLOSE,
	GTK_BUTTONS_CANCEL,
	GTK_BUTTONS_YES_NO,
	GTK_BUTTONS_OK_CANCEL
} GtkButtonsType;
#endif // __GTK_H__

/* Glib prototypes */
typedef guint (*GIDLEADDPROC)(GSourceFunc function, gpointer data);
typedef GTypeInstance *(*GTYPECHECKINSTANCECASTPROC)(GTypeInstance *instance, GType iface_type);

/* Gtk prototypes */
typedef void (*GTKINITPROC)(int *argc, char ***argv);
typedef gboolean (*GTKINITCHECKPROC)(int *argc, char ***argv);
typedef void (*GTKMAINPROC)(void);
typedef void (*GTKMAINQUITPROC)(void);
typedef GType (*GTKWINDOWGETTYPEPROC)(void);
typedef void (*GTKWINDOWSETTITLEPROC)(GtkWindow *window, const gchar *title);
typedef gint (*GTKDIALOGRUNPROC)(GtkDialog *dialog);
typedef GType (*GTKDIALOGGETTYPEPROC)(void);
typedef GtkWidget *(*GTKDIALOGGETWIDGETFORRESPONSEPROC)(GtkDialog *dialog, gint response_id);
typedef void (*GTKWIDGETDESTROYPROC)(GtkWidget *widget);
typedef GType (*GTKBUTTONGETTYPEPROC)(void);
typedef void (*GTKBUTTONSETLABELPROC)(GtkButton *button, const gchar *label);
typedef GtkWidget *(*GTKMESSAGEDIALOGNEWPROC)(GtkWindow *parent, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ...);

/* Implementation bits */
#ifndef G_DISABLE_CAST_CHECKS
#define _DYN_G_TYPE_CIC(ip, gt, ct) ((ct *)dynGnome.g_type_check_instance_cast((GTypeInstance *)ip, gt))
#else
#define _DYN_G_TYPE_CIC(ip, gt, ct) ((ct *)ip)
#endif

#define DYN_G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type) (_DYN_G_TYPE_CIC((instance), (g_type), c_type))

#define DYN_GTK_TYPE_WINDOW (dynGnome.gtk_window_get_type())
#define DYN_GTK_TYPE_DIALOG (dynGnome.gtk_dialog_get_type())
#define DYN_GTK_TYPE_BUTTON (dynGnome.gtk_button_get_type())

#define DYN_GTK_WINDOW(obj) (DYN_G_TYPE_CHECK_INSTANCE_CAST((obj), DYN_GTK_TYPE_WINDOW, GtkWindow))
#define DYN_GTK_DIALOG(obj) (DYN_G_TYPE_CHECK_INSTANCE_CAST((obj), DYN_GTK_TYPE_DIALOG, GtkDialog))
#define DYN_GTK_BUTTON(obj) (DYN_G_TYPE_CHECK_INSTANCE_CAST((obj), DYN_GTK_TYPE_BUTTON, GtkButton))

#ifndef __GTK_H__
#define GTK_WINDOW DYN_GTK_WINDOW
#define GTK_DIALOG DYN_GTK_DIALOG
#define GTK_BUTTON DYN_GTK_BUTTON
#endif

#define GLIB_FUNCS \
	G_FUNC(g_idle_add, GIDLEADDPROC) \
	G_FUNC(g_type_check_instance_cast, GTYPECHECKINSTANCECASTPROC)

#define GTK_FUNCS \
	G_FUNC(gtk_init, GTKINITPROC) \
	G_FUNC(gtk_init_check, GTKINITCHECKPROC) \
	G_FUNC(gtk_main, GTKMAINPROC) \
	G_FUNC(gtk_main_quit, GTKMAINQUITPROC) \
	G_FUNC(gtk_window_get_type, GTKWINDOWGETTYPEPROC) \
	G_FUNC(gtk_window_set_title, GTKWINDOWSETTITLEPROC) \
	G_FUNC(gtk_dialog_run, GTKDIALOGRUNPROC) \
	G_FUNC(gtk_dialog_get_type, GTKDIALOGGETTYPEPROC) \
	G_FUNC(gtk_dialog_get_widget_for_response, GTKDIALOGGETWIDGETFORRESPONSEPROC) \
	G_FUNC(gtk_widget_destroy, GTKWIDGETDESTROYPROC) \
	G_FUNC(gtk_button_get_type, GTKBUTTONGETTYPEPROC) \
	G_FUNC(gtk_button_set_label, GTKBUTTONSETLABELPROC) \
	G_FUNC(gtk_message_dialog_new, GTKMESSAGEDIALOGNEWPROC)

struct GnomeFunctions
{
#define G_FUNC(name, type) type name;
	GLIB_FUNCS
	GTK_FUNCS
#undef G_FUNC
};

#define HAVE_GTK dynGnome.gtk_init

extern GnomeFunctions dynGnome;

void initGnomeFunctions();

#endif // GNOME_FUN_H
