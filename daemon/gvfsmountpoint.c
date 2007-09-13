#include <config.h>
#include <gvfsmountpoint.h>
#include <gvfsdaemonutils.h>

GVfsMountpoint *
g_vfs_mountpoint_copy (GVfsMountpoint  *mountpoint)
{
  GVfsMountpoint *copy;

  copy = g_new (GVfsMountpoint, 1);
  copy->method = g_strdup (mountpoint->method);
  copy->user = g_strdup (mountpoint->user);
  copy->host = g_strdup (mountpoint->host);
  copy->port = mountpoint->port;
  copy->path = g_strdup (mountpoint->path);

  return copy;
}

void
g_vfs_mountpoint_free (GVfsMountpoint  *mountpoint)
{
  g_free (mountpoint->method);
  g_free (mountpoint->user);
  g_free (mountpoint->host);
  g_free (mountpoint->path);
  g_free (mountpoint);
}

GVfsMountpoint *
g_vfs_mountpoint_from_dbus (DBusMessageIter *iter)
{
  GVfsMountpoint *mountpoint;
  dbus_int32_t port;
  char *str;
  char *path_data;
  int path_len;
  DBusMessageIter array;

  mountpoint = g_new0 (GVfsMountpoint, 1);

  if (dbus_message_iter_get_arg_type (iter) != DBUS_TYPE_STRING)
    goto out;

  dbus_message_iter_get_basic (iter, &str);
  mountpoint->method = g_strdup (str);

  if (!dbus_message_iter_next (iter))
    goto out;
  
  if (dbus_message_iter_get_arg_type (iter) != DBUS_TYPE_STRING)
    goto out;

  dbus_message_iter_get_basic (iter, &str);
  mountpoint->user = g_strdup (str);

  if (!dbus_message_iter_next (iter))
    goto out;
  
  if (dbus_message_iter_get_arg_type (iter) != DBUS_TYPE_STRING)
    goto out;

  dbus_message_iter_get_basic (iter, &str);
  mountpoint->host = g_strdup (str);

  if (!dbus_message_iter_next (iter))
    goto out;
  
  if (dbus_message_iter_get_arg_type (iter) != DBUS_TYPE_INT32)
    goto out;

  dbus_message_iter_get_basic (iter, &port);
  mountpoint->port = port;

  if (!dbus_message_iter_next (iter))
    goto out;

  if (dbus_message_iter_get_arg_type (iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type (iter) != DBUS_TYPE_BYTE)
    goto out;

  dbus_message_iter_recurse (iter, &array);
  dbus_message_iter_get_fixed_array (&array, &path_data, &path_len);

  mountpoint->port = port;
  mountpoint->path = g_strndup (path_data, path_len);
  g_free (path_data);
  
  return mountpoint;

 out:
  g_vfs_mountpoint_free (mountpoint);
  return NULL;
}

static void
append_string (DBusMessageIter *iter, char *string)
{
  if (string == NULL)
    string = "";
  
  if (!dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING,
				       &string))
    g_error ("out of memory");
}

static void
append_int (DBusMessageIter *iter, int _val)
{
  dbus_int32_t val;
  val = _val;
  
  if (!dbus_message_iter_append_basic (iter, DBUS_TYPE_INT32, &val))
    g_error ("out of memory");
}

void
g_vfs_mountpoint_to_dbus (GVfsMountpoint  *mountpoint,
			  DBusMessageIter *iter)
{
  append_string (iter, mountpoint->method);
  /* TODO: Is this always utf8? */
  append_string (iter, mountpoint->user);
  append_string (iter, mountpoint->host);
  append_int (iter, mountpoint->port);
  _g_dbus_message_iter_append_filename (iter, mountpoint->path);
}
