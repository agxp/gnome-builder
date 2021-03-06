/* ide-fixit.c
 *
 * Copyright © 2015 Christian Hergert <christian@hergert.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dazzle.h"

#include "diagnostics/ide-fixit.h"
#include "diagnostics/ide-source-range.h"

G_DEFINE_BOXED_TYPE (IdeFixit, ide_fixit, ide_fixit_ref, ide_fixit_unref)

struct _IdeFixit
{
  volatile gint   ref_count;
  IdeSourceRange *range;
  gchar          *text;
};

DZL_DEFINE_COUNTER (instances, "IdeFixit", "Instances", "Number of fixit instances")

IdeFixit *
ide_fixit_new (IdeSourceRange *source_range,
               const gchar    *replacement_text)
{
  IdeFixit *self;

  g_return_val_if_fail (source_range, NULL);
  g_return_val_if_fail (replacement_text, NULL);

  self = g_slice_new0 (IdeFixit);
  self->ref_count = 1;
  self->range = ide_source_range_ref (source_range);
  self->text = g_strdup (replacement_text);

  DZL_COUNTER_INC (instances);

  return self;
}

static void
ide_fixit_destroy (IdeFixit *self)
{
  g_clear_pointer (&self->range, ide_source_range_unref);
  g_clear_pointer (&self->text, g_free);
  g_slice_free (IdeFixit, self);

  DZL_COUNTER_DEC (instances);
}

IdeFixit *
ide_fixit_ref (IdeFixit *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count > 0, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
ide_fixit_unref (IdeFixit *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count > 0);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    ide_fixit_destroy (self);
}

void
ide_fixit_apply (IdeFixit *self)
{
}

/**
 * ide_fixit_get_text:
 * @self: an #IdeFixit.
 *
 * Gets the text to replace the source range with.
 *
 * Returns: A string with the replacement text.
 */
const gchar *
ide_fixit_get_text (IdeFixit *self)
{
  g_return_val_if_fail (self, NULL);

  return self->text;
}

/**
 * ide_fixit_get_range:
 * @self: an #IdeFixit.
 *
 * Gets the range for the replacement text. The range is non inclusive of the
 * end location. [a,b)
 *
 * Returns: (transfer none): An #IdeSourceRange.
 */
IdeSourceRange *
ide_fixit_get_range (IdeFixit *self)
{
  g_return_val_if_fail (self, NULL);

  return self->range;
}
