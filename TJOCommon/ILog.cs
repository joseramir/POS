using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bitnet.TJOCommon
{
    /// <summary>
    /// Interface que describe los metodos esperados en un proveedor de log de comunicaciones.
    /// </summary>
    public interface ILog
    {
        /// <summary>
        /// Metodo que se llama cuando se quiere escribir algo en el LOG.
        /// </summary>
        /// <param name="msg">El mensaje que se quiere mandar al log</param>
        void Write(string msg);
    }
}
