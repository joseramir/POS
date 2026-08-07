#include "stdafx.h"

#ifdef POSXP
using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// La información general sobre un ensamblado se controla mediante el siguiente
// conjunto de atributos. Cambie estos atributos para modificar la información
// asociada con un ensamblado.
//
[assembly:AssemblyTitleAttribute("PosXP")];
[assembly:AssemblyDescriptionAttribute("Pos para Windows XP con soporte TouchScreen")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("ALBERDI S.A.")];
[assembly:AssemblyProductAttribute("posxp")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) ALBERDI S.A. 2008/2009")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// La información de versión de un ensamblado consta de los cuatro valores siguientes:
//
//      Versión principal
//      Versión secundaria
//      Número de versión de compilación
//      Revisión
//
// Puede especificar todos los valores o usar los valores predeterminados de número de versión de compilación y de revisión
// mediante el asterisco ('*'), como se muestra a continuación:

[assembly:AssemblyVersionAttribute("3.0.4573")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
#endif