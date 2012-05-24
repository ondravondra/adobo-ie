#include <Windows.h>
#include <fstream>
#include "version.h"

using namespace std;

#if VER_NUMBER_BUILD < 0
#define MANIFEST_VERSION VERSTR4(VER_NUMBER_MAJOR, VER_NUMBER_MINOR, VER_NUMBER_RELEASE, 65535)
#else
#define MANIFEST_VERSION _VER_STRVER
#endif

const char *xmlIndent = "    ";
const char *xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
const char *xml2 = "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">";
const char *xmlTail = "</assembly>";

const char *manifestVersion = VERSTR(MANIFEST_VERSION);

void WriteMagpieIdent()
{
  ofstream f;
  f.open("magpie.ident.manifest");

  f << xml1 << endl;
  f << xml2 << endl << endl;
  f << xmlIndent << "<assemblyIdentity type=\"win32\" name=\"magpie\" version=\"" << manifestVersion << "\" />" << endl;
  f << endl << xmlTail << endl;
  f.close();
}

void WriteScriptServiceIdent()
{
  ofstream f;
  f.open("scriptservice.ident.manifest");

  f << xml1 << endl;
  f << xml2 << endl << endl;
  f << xmlIndent << "<assemblyIdentity type=\"win32\" name=\"scriptservice\" version=\"" << manifestVersion << "\" />" << endl << endl;
  f << xmlIndent << "<dependency>" << endl;
  f << xmlIndent << xmlIndent << "<dependentAssembly>" << endl;
  f << xmlIndent << xmlIndent << xmlIndent << "<assemblyIdentity type=\"win32\" name=\"magpie\" version=\"" << manifestVersion << "\" language=\"*\" />" << endl;
  f << xmlIndent << xmlIndent << "</dependentAssembly>" << endl;
  f << xmlIndent << "</dependency>" << endl;
  f << endl << xmlTail << endl;
  f.close();
}

void WriteAdoboVersionHeader()
{
  ofstream f;
  f.open("adoboversion.h");
  f << "#define ADOBO_VERSION \"" << manifestVersion << "\"" << endl;
#ifdef DISABLE_REGFREE_ACTIVATION
  f << "#define ADOBO_DISABLE_REGFREE_ACTIVATION" << endl;
#endif
  f.close();
}

int main(int argc, const char ** argv)
{
  if (argc < 2)
  {
    return -1;
  }

  if (!SetCurrentDirectory(argv[1]))
  {
    return -1;
  }

  WriteMagpieIdent();
  WriteScriptServiceIdent();

  char path[MAX_PATH + 1];
  if (!GetModuleFileName(NULL, path, MAX_PATH))
  {
    return -1;
  }

  char * lastSlash = strrchr(path, '\\');
  if (lastSlash)
  {
    *lastSlash = 0;
  }

  if (!SetCurrentDirectory(path))
  {
    return -1;
  }

  WriteAdoboVersionHeader();
  return 0;
}