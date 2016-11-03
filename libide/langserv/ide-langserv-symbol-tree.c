/* ide-langserv-symbol-tree.c
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "ide-langserv-symbol-tree"

#include "ide-langserv-symbol-node.h"
#include "ide-langserv-symbol-tree.h"

typedef struct
{
  GPtrArray *symbols;
} IdeLangservSymbolTreePrivate;

struct _IdeLangservSymbolTree
{
  GObject parent_instance;
};

static void symbol_tree_iface_init (IdeSymbolTreeInterface *iface);

G_DEFINE_TYPE_WITH_CODE (IdeLangservSymbolTree, ide_langserv_symbol_tree, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (IdeLangservSymbolTree)
                         G_IMPLEMENT_INTERFACE (IDE_TYPE_SYMBOL_TREE, symbol_tree_iface_init))

static guint
ide_langserv_symbol_tree_get_n_children (IdeSymbolTree *tree,
                                         IdeSymbolNode *parent)
{
  IdeLangservSymbolTree *self = (IdeLangservSymbolTree *)tree;
  IdeLangservSymbolTreePrivate *priv = ide_langserv_symbol_tree_get_instance_private (self);
  const gchar *parent_name = NULL;
  guint n_children = 0;

  g_assert (IDE_IS_LANGSERV_SYMBOL_TREE (self));
  g_assert (!parent || IDE_IS_SYMBOL_NODE (parent));

  if (IDE_IS_LANGSERV_SYMBOL_NODE (parent))
    parent_name = ide_symbol_node_get_name (parent);

  /*
   * This is all O(n) below, but with the size of trees we are working with
   * its not all that bad. If it becomes an issue, we can move to something
   * like a hashtable.
   */

  for (guint i = 0; i < priv->symbols->len; i++)
    {
      IdeLangservSymbolNode *node = g_ptr_array_index (priv->symbols, i);
      const gchar *node_parent_name = ide_langserv_symbol_node_get_parent_name (node);

      if (g_strcmp0 (node_parent_name, parent_name) == 0)
        n_children++;
    }

  return n_children;
}

static IdeSymbolNode *
ide_langserv_symbol_tree_get_nth_child (IdeSymbolTree *tree,
                                        IdeSymbolNode *parent,
                                        guint          nth)
{
  IdeLangservSymbolTree *self = (IdeLangservSymbolTree *)tree;
  IdeLangservSymbolTreePrivate *priv = ide_langserv_symbol_tree_get_instance_private (self);
  const gchar *parent_name = NULL;

  g_return_val_if_fail (IDE_IS_LANGSERV_SYMBOL_TREE (self), NULL);
  g_return_val_if_fail (!parent || IDE_IS_SYMBOL_NODE (parent), NULL);
  g_return_val_if_fail (nth < priv->symbols->len, NULL);

  if (IDE_IS_LANGSERV_SYMBOL_NODE (parent))
    parent_name = ide_symbol_node_get_name (parent);

  for (guint i = 0; i < priv->symbols->len; i++)
    {
      IdeLangservSymbolNode *node = g_ptr_array_index (priv->symbols, i);
      const gchar *node_parent_name = ide_langserv_symbol_node_get_parent_name (node);

      if (g_strcmp0 (node_parent_name, parent_name) == 0)
        {
          if (nth == 0)
            return g_object_ref (node);
          nth--;
        }
    }

  return NULL;
}

static void
symbol_tree_iface_init (IdeSymbolTreeInterface *iface)
{
  iface->get_n_children = ide_langserv_symbol_tree_get_n_children;
  iface->get_nth_child = ide_langserv_symbol_tree_get_nth_child;
}

static void
ide_langserv_symbol_tree_finalize (GObject *object)
{
  IdeLangservSymbolTree *self = (IdeLangservSymbolTree *)object;
  IdeLangservSymbolTreePrivate *priv = ide_langserv_symbol_tree_get_instance_private (self);

  g_clear_pointer (&priv->symbols, g_ptr_array_unref);

  G_OBJECT_CLASS (ide_langserv_symbol_tree_parent_class)->finalize (object);
}

static void
ide_langserv_symbol_tree_class_init (IdeLangservSymbolTreeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = ide_langserv_symbol_tree_finalize;
}

static void
ide_langserv_symbol_tree_init (IdeLangservSymbolTree *self)
{
}

/**
 * ide_langserv_symbol_tree_new:
 * @symbols: (transfer container) (element-type Ide.LangservSymbolNode): The symbols
 *
 * Creates a new #IdeLangservSymbolTree but takes ownership of @ar.
 *
 * Returns: (transfer full): A newly allocated #IdeLangservSymbolTree.
 */
IdeLangservSymbolTree *
ide_langserv_symbol_tree_new (GPtrArray *symbols)
{
  IdeLangservSymbolTreePrivate *priv;
  IdeLangservSymbolTree *self;

  g_return_val_if_fail (symbols != NULL, NULL);

  self = g_object_new (IDE_TYPE_LANGSERV_SYMBOL_TREE, NULL);
  priv = ide_langserv_symbol_tree_get_instance_private (self);
  priv->symbols = symbols;

  return self;
}
