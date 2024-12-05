// see https://stackoverflow.com/questions/64749385/predefined-type-system-runtime-compilerservices-isexternalinit-is-not-defined
// record classes cause this
namespace System.Runtime.CompilerServices {
    internal static class IsExternalInit {
    }
}