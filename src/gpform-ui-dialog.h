/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2011 Edward C. Hennessy
 * Copyright (C) 2011 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

/*! \file gpform-ui-dialog.h
 */

#define GPFORM_TYPE_UI_DIALOG (gpform_ui_dialog_get_type())
#define GPFORM_UI_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPFORM_TYPE_UI_DIALOG,GPFormUIDialog))
#define GPFORM_UI_DIALOG_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPFORM_TYPE_UI_DIALOG,GPFormUIDialogClass))
#define GPFORM_IS_UI_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPFORM_TYPE_UI_DIALOG))
#define GPFORM_IS_UI_DIALOG_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPFORM_TYPE_UI_DIALOG))
#define GPFORM_UI_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPFORM_TYPE_UI_DIALOG,GPFormUIDialogClass))

/*! \extends GObject
 */
struct _GPFormUIDialog
{
    GtkDialog parent;
};

/*! \private
 */
struct _GPFormUIDialogClass
{
    GtkDialogClass parent;
};

/*! \private */
GType
gpform_ui_dialog_get_type(void);

/*! \brief Set the help spec
 *
 *  The application uses the help spec to locate the help associated with this
 *  dialog.
 *
 *  The help spec has not bee finalized, it could be an identifier, filename,
 *  URL, etc...
 *
 *  The caller must use g_free on the returned string when no longer needed.
 *
 *  \param [in] dialog This dialog
 *  \return The specification for the help associated with this dialog.
 */
gchar*
gpform_ui_dialog_get_help_spec(const GPFormUIDialog *dialog);

/*! \brief Get the model
 *
 *  The caller must use _object_unref on the returned pointer when
 *  no longer needed.
 *
 *  \param [in,out] dialog This dialog
 *  \return The presentation model for this dialog
 */
GPFormPMDialog*
gpform_ui_dialog_get_model(const GPFormUIDialog *dialog);

/*! \brief Set the model name
 *
 *  The caller must use g_free on the returned string when no longer needed.
 *
 *  \param [in] dialog This dialog
 *  \return The name of the model associated with this dialog
 */
gchar*
gpform_ui_dialog_get_model_name(const GPFormUIDialog *dialog);

/*! \brief Get a screenshot of the dialog box.
 *
 *  This function gets an image of the dialog box.
 *
 *  When no longer needed, the caller must call g_object_unref() on the returned
 *  pointer to free the resources associated with the GdkPixbuf.
 *
 *  \param [in] dialog The widget to snapshot
 *  \return TRUE if the image was successfully written.
 */
int
gpform_ui_dialog_get_snapshot(const GPFormUIDialog* dialog);

/*! \brief Set the help spec
 *
 *  The application uses the help spec to locate the help associated with this
 *  dialog.
 *
 *  The help spec has not bee finalized, it could be an identifier, filename,
 *  URL, etc...
 *
 *  \param [in,out] dialog This dialog
 *  \param [in] model_name The specification for the help associated with this dialog
 */
void
gpform_ui_dialog_set_help_spec(GPFormUIDialog *dialog, const gchar *help_spec);

/*! \brief Set the model
 *
 *  \param [in,out] dialog This dialog
 *  \param [in]     model  The presentation model for this dialog
 */
void
gpform_ui_dialog_set_model(GPFormUIDialog *dialog, GPFormPMDialog *model);

/*! \brief Set the model name
 *
 *  \param [in,out] dialog This dialog
 *  \param [in] model_name The name of the model associated with this dialog
 */
void
gpform_ui_dialog_set_model_name(GPFormUIDialog *dialog, const gchar *model_name);

