using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Prominente
{
    /// <summary>
    /// Manejo de los mensajes de error disponibles, segun la documentacion recibida.
    /// </summary>
    static class Error
    {
        /// <summary>
        /// Lista estatica del significado de los mensajes de error.
        /// </summary>
        static string[] msgs = 
        {
            "La transacción se registró correctamente", 
            "El código de empresa es requerido", 
            "El código de empresa debe ser numérico", 
            "El código de empresa no puede superar los 10 caracteres", 
            "El código de agente es requerido", 
            "El código de agente debe ser numérico", 
            "El código de agente no puede superar los 10 caracteres", 
            "El número de celular es requerido", 
            "El número de celular debe ser numérico", 
            "El número de celular debe ser de 10 dígitos", 
            "El importe es requerido", 
            "El importe debe ser numérico", 
            "El código de negocio es requerido", 
            "El código de negocio debe ser numérico", 
            "El código de empresa es inválido", 
            "El código de agente es inválido", 
            "El importe es inválido", 
            "El código de negocio es inválido", 
            "No dispone de saldo para realizar la transacción", 
            "No se pudo establecer conexión con el proveedor", 
            "Se produjo un error al procesar la respuesta del proveedor", 
            "No se pudo cargar la respuesta del proveedor", 
            "Error al generar la petición", 
            "Error al enviar la petición al proveedor", 
            "Se produjo un error en el proveeedor, por favor intente nuevamente", 
            "Se produjo un error al procesar la petición, por favor intente nuevamente", 
            "Se ha producido un error inesperado en la aplicación, por favor intente nuevamente", 
            "Se produjo un error al validar la transacción", 
            "Se produjo un error al registrar la transacción", 
            "Se produjo un error al procesar la transacción", 
            "El campo PIN no tiene el formato requerido", 
            "El campo canal no tiene el formato requerido", 
            "El campo cargas diarias no tiene el formato requerido", 
            "El campo cargas mensuales no tiene el formato requerido requerido", 
            "El campo máxima carga no tiene el formato requerido", 
            "No se puede cargar el XML de validación", 
            "Se ha excedido el monto máximo de carga", 
            "Se ha excedido el límite de cargas diarias", 
            "Se ha excedido el límite de cargas mensuales", 
            "Ya se ha realizado una recarga para esa línea, por favor intente más tarde", 
            "El número ingresado no pertenece a una línea existente", 
            "El número ingresado no pertenece a una línea activa", 
            "El número ingresado no pertenece a una línea prepaga", 
            "La línea supera la cantidad de cargas permitidas por día", 
            "La línea supera el monto en cargas permitidas por día", 
            "La línea no se encuentra habilitada para realizar la recarga"
        };

        /// <summary>
        /// Retorna el mensaje de error asociado con el codigo de error que se le indica.
        /// </summary>
        /// <param name="codigo">El codigo de error para el que se quiere el mensaje correspondiente.</param>
        /// <returns>El mensaje que corresponde al codigo de error recibido</returns>
        public static string Mensaje(int codigo)
        {
            string msg = (codigo < 0 || codigo >= msgs.Length) ? "Error Desconocido" : msgs[codigo];
            return msg + " (" + codigo.ToString() + ")";
        }
    }
}
