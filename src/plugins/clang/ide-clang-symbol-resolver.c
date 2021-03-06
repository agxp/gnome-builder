/* ide-clang-symbol-resolver.c
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

#define G_LOG_DOMAIN "clang-symbol-resolver"

#include "ide-clang-service.h"
#include "ide-clang-symbol-resolver.h"

struct _IdeClangSymbolResolver
{
  IdeObject parent_instance;
};

static void symbol_resolver_iface_init (IdeSymbolResolverInterface *iface);

G_DEFINE_TYPE_EXTENDED (IdeClangSymbolResolver, ide_clang_symbol_resolver, IDE_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (IDE_TYPE_SYMBOL_RESOLVER,
                                               symbol_resolver_iface_init))

static void
ide_clang_symbol_resolver_lookup_symbol_cb (GObject      *object,
                                            GAsyncResult *result,
                                            gpointer      user_data)
{
  IdeClangService *service = (IdeClangService *)object;
  g_autoptr(IdeClangTranslationUnit) unit = NULL;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeSymbol) symbol = NULL;
  g_autoptr(GError) error = NULL;
  IdeSourceLocation *location;

  g_assert (IDE_IS_CLANG_SERVICE (service));
  g_assert (G_IS_TASK (task));

  location = g_task_get_task_data (task);

  unit = ide_clang_service_get_translation_unit_finish (service, result, &error);

  if (unit == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&error));
      return;
    }

  symbol = ide_clang_translation_unit_lookup_symbol (unit, location, &error);

  if (symbol == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&error));
      return;
    }

  g_task_return_pointer (task, g_steal_pointer (&symbol), (GDestroyNotify)ide_symbol_unref);
}

static void
ide_clang_symbol_resolver_lookup_symbol_async (IdeSymbolResolver   *resolver,
                                               IdeSourceLocation   *location,
                                               GCancellable        *cancellable,
                                               GAsyncReadyCallback  callback,
                                               gpointer             user_data)
{
  IdeClangSymbolResolver *self = (IdeClangSymbolResolver *)resolver;
  IdeClangService *service = NULL;
  IdeContext *context;
  IdeFile *file;
  g_autoptr(GTask) task = NULL;

  IDE_ENTRY;

  g_assert (IDE_IS_CLANG_SYMBOL_RESOLVER (self));
  g_assert (location != NULL);

  context = ide_object_get_context (IDE_OBJECT (self));
  service = ide_context_get_service_typed (context, IDE_TYPE_CLANG_SERVICE);
  file = ide_source_location_get_file (location);

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_priority (task, G_PRIORITY_LOW);
  g_task_set_task_data (task, ide_source_location_ref (location),
                        (GDestroyNotify)ide_source_location_unref);

  ide_clang_service_get_translation_unit_async (service,
                                                file,
                                                0,
                                                cancellable,
                                                ide_clang_symbol_resolver_lookup_symbol_cb,
                                                g_steal_pointer (&task));

  IDE_EXIT;
}

static IdeSymbol *
ide_clang_symbol_resolver_lookup_symbol_finish (IdeSymbolResolver  *resolver,
                                                GAsyncResult       *result,
                                                GError            **error)
{
  IdeSymbol *ret;
  GTask *task = (GTask *)result;

  IDE_ENTRY;

  g_return_val_if_fail (IDE_IS_CLANG_SYMBOL_RESOLVER (resolver), NULL);
  g_return_val_if_fail (G_IS_TASK (task), NULL);

  ret = g_task_propagate_pointer (task, error);

  IDE_RETURN (ret);
}

static void
ide_clang_symbol_resolver_get_symbol_tree_cb2 (GObject      *object,
                                               GAsyncResult *result,
                                               gpointer      user_data)
{
  IdeClangTranslationUnit *unit = (IdeClangTranslationUnit *)object;
  g_autoptr(IdeSymbolTree) ret = NULL;
  g_autoptr(GTask) task = user_data;
  g_autoptr(GError) error = NULL;

  ret = ide_clang_translation_unit_get_symbol_tree_finish (unit, result, &error);

  if (ret == NULL)
    g_task_return_error (task, g_steal_pointer (&error));
  else
    g_task_return_pointer (task, g_steal_pointer (&ret), g_object_unref);
}

static void
ide_clang_symbol_resolver_get_symbol_tree_cb (GObject      *object,
                                              GAsyncResult *result,
                                              gpointer      user_data)
{
  IdeClangService *service = (IdeClangService *)object;
  g_autoptr(IdeClangTranslationUnit) unit = NULL;
  g_autoptr(GTask) task = user_data;
  g_autoptr(GError) error = NULL;
  GFile *file;

  IDE_ENTRY;

  g_assert (IDE_IS_CLANG_SERVICE (service));
  g_assert (G_IS_TASK (task));

  unit = ide_clang_service_get_translation_unit_finish (service, result, &error);

  if (unit == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&error));
      IDE_EXIT;
    }

  file = g_task_get_task_data (task);
  g_assert (G_IS_FILE (file));

  ide_clang_translation_unit_get_symbol_tree_async (unit,
                                                    file,
                                                    g_task_get_cancellable (task),
                                                    ide_clang_symbol_resolver_get_symbol_tree_cb2,
                                                    g_object_ref (task));

  IDE_EXIT;
}

static void
ide_clang_symbol_resolver_get_symbol_tree_async (IdeSymbolResolver   *resolver,
                                                 GFile               *file,
                                                 IdeBuffer           *buffer,
                                                 GCancellable        *cancellable,
                                                 GAsyncReadyCallback  callback,
                                                 gpointer             user_data)
{
  IdeClangSymbolResolver *self = (IdeClangSymbolResolver *)resolver;
  g_autoptr(GTask) task = NULL;
  g_autoptr(IdeFile) ifile = NULL;
  IdeClangService *service;
  IdeContext *context;

  IDE_ENTRY;

  g_assert (IDE_IS_CLANG_SYMBOL_RESOLVER (self));
  g_assert (G_IS_FILE (file));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  context = ide_object_get_context (IDE_OBJECT (self));
  service = ide_context_get_service_typed (context, IDE_TYPE_CLANG_SERVICE);

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_priority (task, G_PRIORITY_LOW);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);

  ifile = ide_file_new (context, file);

  ide_clang_service_get_translation_unit_async (service,
                                                ifile,
                                                0,
                                                cancellable,
                                                ide_clang_symbol_resolver_get_symbol_tree_cb,
                                                g_steal_pointer (&task));

  IDE_EXIT;
}

static IdeSymbolTree *
ide_clang_symbol_resolver_get_symbol_tree_finish (IdeSymbolResolver  *resolver,
                                                  GAsyncResult       *result,
                                                  GError            **error)
{
  IdeSymbolTree *ret;
  GTask *task = (GTask *)result;

  IDE_ENTRY;

  g_return_val_if_fail (IDE_IS_CLANG_SYMBOL_RESOLVER (resolver), NULL);
  g_return_val_if_fail (G_IS_TASK (task), NULL);

  ret = g_task_propagate_pointer (task, error);

  IDE_RETURN (ret);
}

static void
ide_clang_symbol_resolver_find_scope_cb (GObject      *object,
                                         GAsyncResult *result,
                                         gpointer      user_data)
{
  IdeClangService *service = (IdeClangService *)object;
  g_autoptr(IdeClangTranslationUnit) unit = NULL;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeSymbol) symbol = NULL;
  g_autoptr(GError) error = NULL;
  IdeSourceLocation *location;

  g_assert (IDE_IS_CLANG_SERVICE (service));
  g_assert (G_IS_ASYNC_RESULT (result));
  g_assert (G_IS_TASK (task));

  unit = ide_clang_service_get_translation_unit_finish (service, result, &error);

  if (unit == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&error));
      return;
    }

  location = g_task_get_task_data (task);
  symbol = ide_clang_translation_unit_find_nearest_scope (unit, location, &error);

  if (symbol == NULL)
    g_task_return_error (task, g_steal_pointer (&error));
  else
    g_task_return_pointer (task,
                           g_steal_pointer (&symbol),
                           (GDestroyNotify) ide_symbol_unref);
}

static void
ide_clang_symbol_resolver_find_nearest_scope_async (IdeSymbolResolver   *symbol_resolver,
                                                    IdeSourceLocation   *location,
                                                    GCancellable        *cancellable,
                                                    GAsyncReadyCallback  callback,
                                                    gpointer             user_data)
{
  IdeClangSymbolResolver *self = (IdeClangSymbolResolver *)symbol_resolver;
  g_autoptr(GTask) task = NULL;
  IdeClangService *service;
  IdeContext *context;
  IdeFile *file;

  IDE_ENTRY;

  g_return_if_fail (IDE_IS_CLANG_SYMBOL_RESOLVER (self));
  g_return_if_fail (location != NULL);

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_priority (task, G_PRIORITY_LOW);
  g_task_set_source_tag (task, ide_clang_symbol_resolver_find_nearest_scope_async);
  g_task_set_task_data (task,
                        ide_source_location_ref (location),
                        (GDestroyNotify) ide_source_location_unref);

  context = ide_object_get_context (IDE_OBJECT (self));
  service = ide_context_get_service_typed (context, IDE_TYPE_CLANG_SERVICE);
  file = ide_source_location_get_file (location);

  ide_clang_service_get_translation_unit_async (service,
                                                file,
                                                0,
                                                cancellable,
                                                ide_clang_symbol_resolver_find_scope_cb,
                                                g_steal_pointer (&task));

  IDE_EXIT;
}

static IdeSymbol *
ide_clang_symbol_resolver_find_nearest_scope_finish (IdeSymbolResolver  *resolver,
                                                     GAsyncResult       *result,
                                                     GError            **error)
{
  IdeSymbol *ret;

  IDE_ENTRY;

  g_return_val_if_fail (IDE_IS_CLANG_SYMBOL_RESOLVER (resolver), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);

  ret = g_task_propagate_pointer (G_TASK (result), error);

  IDE_RETURN (ret);
}

static void
ide_clang_symbol_resolver_class_init (IdeClangSymbolResolverClass *klass)
{
}

static void
symbol_resolver_iface_init (IdeSymbolResolverInterface *iface)
{
  iface->lookup_symbol_async = ide_clang_symbol_resolver_lookup_symbol_async;
  iface->lookup_symbol_finish = ide_clang_symbol_resolver_lookup_symbol_finish;
  iface->get_symbol_tree_async = ide_clang_symbol_resolver_get_symbol_tree_async;
  iface->get_symbol_tree_finish = ide_clang_symbol_resolver_get_symbol_tree_finish;
  iface->find_nearest_scope_async = ide_clang_symbol_resolver_find_nearest_scope_async;
  iface->find_nearest_scope_finish = ide_clang_symbol_resolver_find_nearest_scope_finish;
}

static void
ide_clang_symbol_resolver_init (IdeClangSymbolResolver *self)
{
}
