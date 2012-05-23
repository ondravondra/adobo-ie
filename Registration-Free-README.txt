How to use the adobo library without its registration.

Shared dll registration sucks, fortunatelly COM libraries can be loaded without it on WinXP+.

We use these mechanisms:

XML manifests

XML side by side manifests embedded in adobo shared libraries. These manifests contain description of the library and all com classes and interfaces with their uids.
See http://msdn.microsoft.com/en-us/library/windows/desktop/aa374219%28v=vs.85%29.aspx for description of the manifests.

This is sufficient to make background service exe server work because in ScriptServiceFactory.h there is comment telling the linker to add manifest dependency.
For the BHO library loaded into iexplore.exe this would not be sufficient.

Activation API

http://msdn.microsoft.com/en-us/library/windows/desktop/aa376620%28v=vs.85%29.aspx

This is used to load xml manifests when the extension library needs to access a typelibrary from adobo.
There is CActivationHelper class in libcomhelper which retrieves the manifest from the bho module. This manifest contains dependency declarations pointing to adobo dlls.
Those dlls should be placed in the same directory as the bho module.

The api is used when instantiating adobo coclasses using CoCreateInstance and when querying interfaces.
It would also have to be used for retrieving typeinfos in IDispatchImpl classes if the IDispatchImpl would be located in bho dll and it would implement an interface from adobo.
In this case the IDispatchImpl would have to be wrapped so its methods would get the activated context.

ISOLATION_AWARE_ENABLED defined in stdafx.h

It causes com apis to be wrapped so first the manifest is loaded from the calling module (our bho).
There is manifestdependency defined for the bho which is set in SalsitaScriptedClient.h and it contains references to adobo libraries.
Theoretically it should make activation api unnecessary but it is not wrapping calls inside atl classes and without activation api it did not suffice in all cases.

