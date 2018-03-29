/*
*  Copyright (C) 2003 Kevin Harris
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful, but
*  WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*  As a special exception, source code generated by this program is not
*  neccessarily automatically covered by the GPL.  You are free to speficy a
*  license, so that it will generate code with the license of your choice,
*  providing all copyright and license information for this program remains
*  intact.
*/

/*
  $Id: gen-class.cpp,v 1.5 2008/06/09 16:31:17 kpharris Exp $
 */

/*
  TODO:

  * Allow parents to be in different namespaces (parent namespaces are
    currently stripped and ignored)
  * Allow parents to have template parameters (they are currently determined,
    but unused).
  * Allow setting of default values to the template parameters.
 */

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdio>
#include <cstdlib>

// Put your name here...
static const string AuthorName = "Kevin Harris";
// Put your email address here...
static const string EmailText = "kpharris@users.sourceforge.net";
// Change the package description as required.
static const string PackageText = "Part of \"Amethyst\" -- A playground for graphics development.";
static const string PackageLocation = "http://sourceforge.net/projects/manygames";
static const string HeaderExtension = "hpp";

static const string GPL_License = "\
 * This program is free software; you can redistribute it and/or modify\n\
 * it under the terms of the GNU General Public License as published by\n\
 * the Free Software Foundation; either version 2 of the License, or\n\
 * (at your option) any later version.\n\
 *\n\
 * This program is distributed in the hope that it will be useful, but\n\
 * WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n\
 * General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU General Public License\n\
 * along with this program; if not, write to the Free Software\n\
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.\n";

static const string LGPL_License = "\
 * This library is free software; you can redistribute it and/or\n\
 * modify it under the terms of the GNU Lesser General Public\n\
 * License as published by the Free Software Foundation; either\n\
 * version 2.1 of the License, or (at your option) any later version.\n\
 *\n\
 * This library is distributed in the hope that it will be useful,\n\
 * but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n\
 * Lesser General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU Lesser General Public\n\
 * License along with this library; if not, write to the Free Software\n\
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.\n";


string GenerateFileHeader(const string& license = GPL_License)
{
  static const string CVS_HEADER = string("$") + "Id$";


  time_t current_time;
  tm time_struct;
  time(&current_time);
  localtime_s(&time_struct, &current_time);
  char year_text[6];
  sprintf(year_text, "%d", (time_struct.tm_year) + 1900);


  string header = (string("/*\n") +
                   " * " + CVS_HEADER + "\n *\n" +
                   " * " + PackageText + "\n" +
                   " * Copyright (C) " + year_text + " " + AuthorName + "\n" +
                   " *\n" + license + " */");

  return header;
}

string GenerateTemplateBeforeReturnType(const vector<pair<string,string> >& tparams,
                                        const string& indent)
{
  string return_string;

  if( !tparams.empty() )
  {
    return_string += "template<";
    for( vector<pair<string,string> >::const_iterator tname = tparams.begin();
         tname != tparams.end();
         ++tname )
    {
      if( !tname->first.empty() )
      {
        return_string += tname->first;
      }
      else
      {
        return_string += "class";
      }
      return_string += " ";
      return_string += tname->second;

      if( (tparams.end() - tname) > 1 )
      {
        return_string += ", ";
      }
    }
    return_string += ">\n" + indent;
  }
  return return_string;
}

string GenerateTemplateAfterClassname(const vector<pair<string,string> >& tparams)
{
  string return_string;

  if( !tparams.empty() )
  {
    return_string += "<";
    for( vector<pair<string,string> >::const_iterator tname = tparams.begin();
         tname != tparams.end();
         ++tname )
    {
      return_string += tname->second;

      if( (tparams.end() - tname) > 1 )
      {
        return_string += ",";
      }
    }
    return_string += ">";
  }
  return return_string;
}

string GenerateClassDeclaration(const string& classname,
                                const string& indent = string(""),
                                const string& parentname = string(""),
                                const vector<pair<string,string> >& tparams = vector<pair<string,string> >())
{
  string declaration;


  declaration += "// BEGIN(DELETEME)\n";
declaration += "//-----------------------------------------------------------\n";
  declaration += "#error You must customize this automatically generated class.\n";
  declaration += "// To do this, you must:\n";
  string last_num_used = "1";
  if( !parentname.empty() )
  {
    declaration += ("// (" + last_num_used + ") " +
                    "Provide the proper include for class " + parentname +
                    " (above).\n");
    ++last_num_used[0];
  }
  declaration += ("// (" + last_num_used + ") " +
                  "Enter a class description below, on the line requested.\n");
  ++last_num_used[0];

  declaration += ("// (" + last_num_used + ") " +
                  "Add your own class members to the class.\n");
  ++last_num_used[0];

  declaration += ("// (" + last_num_used + ") " +
                  "Provide the required operations in the copy constructor and assignment\n//  operator");
  if( tparams.empty() )
  {
    declaration += " (in the corresponding .cpp file).\n";
  }
  else
  {
    declaration += ".\n";
  }

  ++last_num_used[0];



  declaration += "//-----------------------------------------------------------\n";
  declaration += "// END(DELETEME)\n\n";

  declaration += indent + "/**\n";
  declaration += indent + " *\n";
  declaration += indent + " * ***CLASS DESCRIPTION GOES HERE*** (multiple lines are ok)\n";
  declaration += indent + " *\n";
  declaration += indent + " * @author " + AuthorName + " <" + EmailText + ">\n";
  declaration += indent + " * @version $" + "Revision: 1.1 $\n";
  declaration += indent + " *\n";
  declaration += indent + " */\n";

  // This will either be used by the template (if any), or the statement after.
  declaration += indent;

  // Generate a template header before the class...
  if( !tparams.empty() )
  {
    declaration += GenerateTemplateBeforeReturnType(tparams, indent);
  }

  // This one gets its indent either from the template test (if any), or the
  // statement right before.
  declaration += "class " + classname;

  if( !parentname.empty() )
  {
    declaration += " : public " + parentname;
  }
  declaration += "\n";
  declaration += indent + "{\n";
  declaration += "\n";
  declaration += indent + "private:\n";
  declaration += "\n";
  declaration += indent + "protected:\n";
  declaration += "\n";
  declaration += indent + "public:\n";
  declaration += indent + "  /** Default constructor */\n";
  declaration += indent + "  " + classname + "();\n";
  declaration += "\n";
  declaration += indent + "  /** Destructor */\n";
  declaration += indent + "  virtual ~" + classname + "();\n";
  declaration += "\n";
  declaration += indent + "  /** Copy constructor */\n";
  declaration += indent + "  " + classname + "(const " + classname + "& old);\n";
  declaration += "\n";
  declaration += indent + "  /** Assignment operator */\n";
  declaration += indent + "  " + classname + "& operator= (const " + classname + "& old);\n";
  declaration += "\n";
  declaration += indent + "}; // class " + classname + "\n";
  return declaration;
} // GenerateClassDeclaration()


string GenerateClassDefinition(const string& classname,
                               const string& indent = string(""),
                               const string& parentname = string(""),
                               const vector<pair<string,string> >& tparams = vector<pair<string,string> >())
{
  string definition;
  string dash_name = string(classname.size(), '-');

  definition += indent + "//-------------------------------" + dash_name + "\n";
  definition += indent + "// Default constructor for class " + classname + "\n";
  definition += indent + "//-------------------------------" + dash_name + "\n";

  definition += (indent +
                 GenerateTemplateBeforeReturnType(tparams, indent) +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "::" + classname + "()");
  if( !parentname.empty() )
  {
    definition += ":\n";
    definition += indent + "  " + parentname + "()";
  }
  definition += "\n";
  definition += indent + "  // Class initializers go here. DELETEME\n";
  definition += indent + "{\n";
  definition += indent + "\n";
  definition += indent + "} // " + classname + "()\n";

  definition += "\n";

  definition += indent + "//----------------------" + dash_name + "\n";
  definition += indent + "// Destructor for class " + classname + "\n";
  definition += indent + "//----------------------" + dash_name + "\n";
  definition += (indent +
                 GenerateTemplateBeforeReturnType(tparams, indent) +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "::~" + classname + "()\n");
  definition += indent + "{\n";
  definition += indent + "\n";
  definition += indent + "} // ~" + classname + "()\n";

  definition += "\n";

  definition += indent + "//----------------------------" + dash_name + "\n";
  definition += indent + "// Copy constructor for class " + classname + "\n";
  definition += indent + "//----------------------------" + dash_name + "\n";
  definition += (indent +
                 GenerateTemplateBeforeReturnType(tparams, indent) +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "::" + classname + "(const " +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "& old)");
  if( !parentname.empty() )
  {
    definition += ":\n";
    definition += indent + "  " + parentname + "(old)";
  }
  definition += "\n";
  definition += indent + "  // Class initializers go here. DELETEME\n";
  definition += indent + "{\n";
  definition += indent + "  // Insert any copy-assignment here. DELETEME\n";
  definition += indent + "} // " + classname + "(" + classname + ")\n";


  definition += "\n";
  definition += indent + "//-------------------------------" + dash_name + "\n";
  definition += indent + "// Assignment operator for class " + classname + "\n";
  definition += indent + "//-------------------------------" + dash_name + "\n";

  definition += (indent +
                 GenerateTemplateBeforeReturnType(tparams, indent) +
                 classname + GenerateTemplateAfterClassname(tparams) + "& " +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "::operator= (const " +
                 classname + GenerateTemplateAfterClassname(tparams) +
                 "& old)\n");
  definition += indent + "{\n";
  definition += indent + "  // Generic check for self-assignment\n";
  definition += indent + "  if( &old != this)\n";
  definition += indent + "  {\n";
  definition += indent + "    // Insert assignment here. DELETEME\n";
  definition += "\n";
  if( !parentname.empty() )
  {
    definition += indent + "    " + parentname + "::operator=(old);\n";
  }
  definition += indent + "  }\n";
  definition += indent + "  return (*this);\n";

  definition += indent + "} // " + classname + "::operator=(" + classname + ")\n";

  return definition;
} // GenerateClassDefinition()


bool GetLicenseFromFile(const string& filename, string& license_text)
{
  ifstream license_input(filename.c_str());

  if( !license_input )
  {
    return false;
  }

  license_text = "";
  while( license_input )
  {
    const int buffer_size = 1024;
    char buffer[1024];

    if( license_input.getline(buffer, buffer_size, '\n') )
    {
      license_text += " * " + string(buffer) + "\n";
    }
  }
  return true;
}

#include <unistd.h>
#if       !defined(_GNU_SOURCE)
#define            _GNU_SOURCE
#endif /* !defined(_GNU_SOURCE) */
#include <getopt.h>

void RemoveSwitches(int& argc, char** argv, string& munged_class, string& parent, string& file_base, string& license_text)
{
  license_text = GPL_License;
  static struct option long_options[] = { {"output", 1, 0, 'o'},
                                          {"parent", 1, 0, 'p'},
                                          {"help", 0, 0, 'h'},
                                          {"version", 0, 0, 'v'},
					  {"license", 0, 0, 'l'},
                                          {0, 0, 0, 0} };

  int c;
  int last_opt = 0;
  while ( 1 )
  {
    int option_index = 0;
    c = getopt_long (argc, argv, "l:o:p:hv", long_options, &option_index);

    // If there aren't any options left... bail.
    if( c == -1 )
      break;

    switch(c)
    {
    case 'l':
      {
	string license_name = string(optarg);
	if( (license_name == "g") || (license_name == "gpl") || (license_name == "GPL") )
	{
	  license_text = GPL_License;
	}
	else if( (license_name == "l") || (license_name == "lgpl") || (license_name == "LGPL") )
	{
	  license_text = LGPL_License;
	}
	else
	{
	  if( !GetLicenseFromFile(license_name, license_text) )
	  {
	    cerr << "Error getting license text from \"" << license_name << "\"" << endl;
	    exit(1);
	  }
	}
      }  break;
    case 'o':
      {
        file_base = string(optarg);
      } break;
    case 'p':
      {
        parent = string(optarg);
      } break;
    case 'v':
      {
        cout << "Class Stub Generator version $Revision: 1.5 $" << endl;
        cout << "Copyright (C) 2003 Kevin Harris" << endl;
        cout << endl;
        cout << "This is free software, released under terms of the GNU Public License (GPL)" << endl;
        cout << "version 2 or later (at your option). See http://www.gnu.org/licenses/gpl.txt " << endl;
        exit(1);
      } break;
    case 'h':
      {
        cout << "Class Stub Generator version $Revision: 1.5 $" << endl;
        cout << "  This is a program to generate class stubs. \n\
  As arguments, it accepts one non-switch argument, which is the classname.\n\
  ie. giving it 'foo.baz' would generate a class baz in namespace foo.\n\
\n\
  The other options are:\n\
  -o --output - specifies the root of the otput filename (no extension)\n\
  -p --parent - specifies a class from which to publicly derive.\n\
  -l --license - specifies a license type (gpl/lgpl) or filename for a license.\n\
\n\
  --version   - displays the version number and copyright/license \n\
  --help      - displays this help \n\
  Notes: If you want templated class output, put your templated list (comma \n\
         deliminated, following the classname (no spaces allowed).  \n\
  Example: \n\
      gen-class namespace.classname,param1,param2=typename,param3=int\n" << endl;
        exit(1);
      } break;
    default:
      {
      } break;
    }
  }

  if( optind < argc )
  {
    munged_class = string(argv[optind]);
  }
}

pair<string,string> ExtractSingleParam(const string& param_tag)
{
  // See the comments in ExtractTemplateParams for the format that we'll accept
  // here.
  pair<string, string> ret_pair;

  if( !param_tag.empty() )
  {
    size_t equals_location = param_tag.find('=', 0);
    string before, after;

    before = param_tag.substr(0, equals_location);

    if( equals_location < param_tag.size() )
    {
      after = param_tag.substr(equals_location + 1,
                               param_tag.size() - equals_location - 1);
    }
    ret_pair.first = after;
    ret_pair.second = before;
  }
  return ret_pair;
}

void ExtractTemplateParams(string& munged_name,
                           vector<pair<string,string> >& template_parameters)
{
  template_parameters.erase(template_parameters.begin(),
                            template_parameters.end());

  // The format I'll accept here (bisonish description):
  // template_param_name:
  //    string                      -- classname
  //    | string ',' template_params
  // ;
  // template_params:
  //    single_param
  //    | template_params ',' single_param
  // ;
  // single_param:
  //    string                      -- name  (type=='class')
  //    | string '=' string         -- name=type
  //    | '=' string                -- type
  // ;

  size_t last_location = 0; // The last location which we've seen a comma

  vector<string> comma_split_strings;

  // While we haven't checked all of the commas in the list...
  while( last_location < munged_name.size() )
  {
    size_t new_location = munged_name.find(',', last_location);
    // The part *before* the comma (if there was a comma).
    string temp = munged_name.substr(last_location,
                                     new_location - last_location);
    comma_split_strings.push_back(temp);
    last_location = new_location;

    // If there was a comma, advance beyond it.
    if( last_location < munged_name.size() )
    {
      ++last_location;
    }
  }

  if( !comma_split_strings.empty() )
  {
    // Extract the name of the class from the comma string list.
    vector<string>::const_iterator current = comma_split_strings.begin();
    munged_name = *current;
    ++current;

    for(; current != comma_split_strings.end(); ++current)
    {
      template_parameters.push_back(ExtractSingleParam(*current));
    }
  }
}

void ExtractNamespaces(string& munged_name, vector<string>& namespaces)
{
  string temp;
  size_t last_location = 0;

  namespaces.erase(namespaces.begin(), namespaces.end());

  while(last_location < munged_name.size())
  {
    size_t new_location = munged_name.find('.', last_location);
    temp = munged_name.substr(last_location, new_location - last_location);
    namespaces.push_back(temp);
    last_location = new_location;
    if( last_location < munged_name.size() )
    {
      ++last_location;
    }
  }
  if( !namespaces.empty() )
  {
    munged_name = namespaces[namespaces.size() - 1];
    namespaces.pop_back();
  }
}

string GenerateNamespaceBegin(const vector<string>& namespaces, string& indent)
{
  string result;
  indent = string("");

  for(vector<string>::const_iterator i = namespaces.begin(); i != namespaces.end(); ++i)
  {
    result += (indent + "namespace " + *i + "\n" +
               indent + "{\n");
    indent += "  ";
  }
  return result;
}

string GenerateNamespaceEnd(const vector<string>& namespaces, string& indent)
{
  string result;

  if( !namespaces.empty() )
  {
    indent = indent.substr(0, (namespaces.size() - 1) * 2);
  }


  for(vector<string>::const_reverse_iterator i = namespaces.rbegin(); i != namespaces.rend(); ++i)
  {
    result += indent + "} // namespace " + *i + "\n";
    indent = indent.substr(0, indent.size() - 2);
  }
  return result;
}

string IncludeGuardMunger(string s)
{
  for(unsigned i = 0; i < s.size(); ++i)
  {
    if( s[i] >= 'a' && s[i] <= 'z')
    {
      s[i] -= 'a' - 'A';
    }
    else if( s[i] == ' ' )
    {
      s[i] = '_';
    }
  }
  return s;
}


string GenerateIncludeGuardText(const vector<string>& namespaces, const string& output_root)
{
  string s = "";
  for( int i = 0; i < int(namespaces.size()); ++i)
  {
    s += namespaces[i] + "  ";
  }
  s += output_root + " " + HeaderExtension + "";
  return IncludeGuardMunger(s);
}

int main(int argc, char** argv)
{
  string classname = "UNNAMED";
  string parent = "";
  string output_root = "CLASSOUT";
  string indent;
  vector<string> namespaces;
  vector<string> parent_namespaces;
  // The order of stuff stored in these next two, as a pair, will be
  // <type, name>
  vector<pair<string, string> > template_arguments;
  vector<pair<string, string> > parent_template_arguments;

  string license;

  RemoveSwitches(argc, argv, classname, parent, output_root, license);

  if( classname == "UNNAMED" )
  {
    cerr << "Error: No class name supplied.  Use --help for help." << endl;
    exit(1);
  }

  // Extract the namespaces and template arguments (if any) from the class name
  // and parent name (if any).
  ExtractNamespaces(classname, namespaces);
  ExtractTemplateParams(classname, template_arguments);
  ExtractNamespaces(parent, parent_namespaces);
  ExtractTemplateParams(parent, parent_template_arguments);

  if( output_root == "CLASSOUT" )
  {
    output_root = classname;
  }

  string namespace_guard_string = GenerateIncludeGuardText(namespaces, output_root);

  ofstream header_output(string(output_root + "." + HeaderExtension).c_str());

  if( !header_output )
  {
    cerr << "Error: Could not open \"" << output_root + "." + HeaderExtension << "\"" << endl;
    exit(1);
  }
  ofstream *cpp_output = NULL;

  if( template_arguments.empty() )
  {
    cpp_output = new ofstream(string(output_root + ".cpp").c_str());
  }

  if( cpp_output && !*cpp_output )
  {
    cerr << "Error: Could not open \"" << output_root + ".cpp\"" << endl;
    exit(1);
  }

  header_output << GenerateFileHeader(license) << endl;
  header_output << endl;
  header_output << "#if       !defined(" << namespace_guard_string << ")" << endl;
  header_output << "#define            " << namespace_guard_string << endl;
  header_output << endl;

  if( !parent.empty() )
  {
    header_output <<  "// --------------------------------------" << endl;
    header_output <<  "// Default include of parent class header" << endl;
    header_output <<  "// --------------------------------------" << endl;
    header_output <<  "#include \"" << parent << "." << HeaderExtension << "\"" << endl;
    header_output << endl;
  }
  header_output << GenerateNamespaceBegin(namespaces, indent) << endl;
  header_output << GenerateClassDeclaration(classname, indent, parent, template_arguments) << endl;

  if( !cpp_output )
  {
    // Must be templated, so we'll dump it in the header!
    header_output << endl;
    header_output << endl;
    header_output << GenerateClassDefinition(classname, indent, parent, template_arguments) << endl;
  }

  header_output << GenerateNamespaceEnd(namespaces, indent) << endl;
  header_output << endl;
  header_output << "#endif /* !defined(" << namespace_guard_string << ") */" << endl;
  header_output << endl;
  header_output.close();


  if( cpp_output )
  {
    *cpp_output << GenerateFileHeader(license) << endl;
    *cpp_output << endl;
    *cpp_output << "#include \"" << output_root << "." << HeaderExtension << "\"" << endl;
    *cpp_output << endl;
    *cpp_output << endl;
    *cpp_output << GenerateNamespaceBegin(namespaces, indent) << endl;
    *cpp_output << GenerateClassDefinition(classname, indent, parent) << endl;
    *cpp_output << GenerateNamespaceEnd(namespaces, indent) << endl;
    cpp_output->close();
  }
}
