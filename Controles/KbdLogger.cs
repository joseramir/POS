using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Pos.Desktop
{
    public static class KbdLogger
    {
        public static bool Enabled { get; set; }

        static KbdLogger()
        {
            Enabled = false;
        }

        public static void Log(string msg)
        {
            if (Enabled)
            {
                File.AppendAllText("kb.log", msg + "\n");
            }
        }

        public static void Log(string[] parts)
        {
            if (Enabled)
            {
                string msg = string.Format("Cargando Tecla (Len={0})", parts.Length);
                bool ac = false;
                foreach (string part in parts)
                {
                    if (ac) msg += ",";
                    ac = true;
                    msg += " '" + part + "'";
                }
                Log(msg);
            }
        }

        public static void Log(Control ctl)
        {
            if (Enabled)
            {
                Log(string.Format("Asignada a: Name='{0}', Tag='{1}', Text='{2}'", ctl.Name, ctl.Tag, ctl.Text));
            }
        }
    }
}
