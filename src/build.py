import os, re

DOC_PREFIX = "////"

class DocObject:
	def __init__(self, info, decl):
		self.Info = info
		self.Declaration = decl
		self.Ctors = 0

def ParseDocComment(lines):
	info = {}
	
	for i in range(len(lines) - 1):
		params = lines[i][len(DOC_PREFIX):].replace("  ", " ").split(" ")
		
		if len(params) > 1:
			if params[0] in info:
				info[params[0]].append(" ".join(params[1:]))
			else:
				info[params[0]] = [" ".join(params[1:])]
	
	decl = lines[len(lines) - 1]

	return DocObject(info, decl)

def HTML(text):
	return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("\"", "&quot;").replace("'", "&#39;")

def AddIf(output, info, key, str):
	if key in info: output.write(str.replace("@info", HTML(info[key][0])))

def FileName(info):
	return info["fullid"][0].replace("::", "_") + ".html"

def Link(output, info, text=""):
	if len(text) == 0: text = info['name'][0]
	output.write(f"<a href=\"./{FileName(info)}\">{HTML(text)}</a>")

def GetClassName(decl):
	match = re.search(r"class\s+([^\s]+)", decl)
	if match == None: return ""
	else: return match.group(1)

def GetStructName(decl):
	match = re.search(r"(?:typedef\s+)?struct\s+([^\s]+)", decl)
	if match == None: return ""
	else: return match.group(1)

def GetAliasNameValue(decl):
	match = re.search(r"typedef\s+([^\s]+\s+)*([^\s;]+);", decl)
	if match == None: return "", ""
	else: return match.group(2), match.group(1).strip()

def GetMacroNameValue(decl):
	match = re.search(r"^#define(?:\s+)([^\s]+)(?:\s+(.*))?$", decl)
	if match == None: return "", ""
	else: return match.group(1), match.group(2)

def GetEnumName(decl):
	match = re.search(r"enum\s+(?:class\s+)?([^\s]+)", decl)
	if match == None: return ""
	else: return match.group(1)

def GetConstNameTyperefValue(decl):
	match = re.search(r"((?:[^\s]+\s+)+)([^\s]+)\s*=\s*([^;]+)\s*;", decl)
	if match == None: return "", "", ""
	else: return match.group(2), match.group(1).strip(), match.group(3).strip()

def GetFieldNameTyperef(decl):
	match = re.search(r"((?:[^\s]+\s+)+)([^\s]+)\s*(?:;|=)", decl)
	if match == None: return "", ""
	else: return match.group(2), match.group(1).strip()

def GetFunctionNameTyperef(decl):
	match = re.search(r"(?:virtual\s+)?((?:[^\s]+\s+)+)([^\s\(]+)\s*\(", decl)
	if match == None: return "", ""
	else: return match.group(2), match.group(1).strip()

def GetInfoValue(info, docLookup):
	if "value" in info: return info["value"][0]
	if "valueref" in info:
		if info["valueref"][0] in docLookup: return GetInfoValue(docLookup[info["valueref"][0]].Info, docLookup)
		else: return info["valueref"][0]
	return ""

def MemberTable(output, docLookup, members, useDeclaration=False, showValue=False):
	members = sorted(members, key=lambda member: member.Info["name"][0])

	output.write("<table>")
	for member in members:
		output.write("<tr><td>")
		if useDeclaration:
			Link(output, member.Info, member.Declaration)
		else:
			Link(output, member.Info)
		output.write("</td><td>")
		if showValue:
			output.write(f"<p>{HTML(GetInfoValue(member.Info, docLookup))}</p></td><td>")
		AddIf(output, member.Info, "shortdesc", "<p>@info</p>")
		output.write("</td></tr>")
	output.write("</table>")

def MemberContent(output, docLookup, children):
	namespaces = []
	ctors = []
	classes = []
	structs = []
	macros = []
	enums = []
	constants = []
	fields = []
	functions = []
	aliases = []

	for member in children:
		type = member.Info["type"][0]
		if type == "namespace":
			namespaces.append(member)
		elif type == "ctor":
			ctors.append(member)
		elif type == "class":
			classes.append(member)
		elif type == "struct":
			structs.append(member)
		elif type == "macro":
			macros.append(member)
		elif type == "enum":
			enums.append(member)
		elif type == "const":
			constants.append(member)
		elif type == "field":
			fields.append(member)
		elif type == "function":
			functions.append(member)
		elif type == "alias":
			aliases.append(member)

	if len(namespaces) > 0:
		output.write("<h2>Namespaces</h2>")
		MemberTable(output, docLookup, namespaces)
	if len(ctors) > 0:
		output.write("<h2>Constructors</h2>")
		MemberTable(output, docLookup, ctors, useDeclaration=True)
	if len(classes) > 0:
		output.write("<h2>Classes</h2>")
		MemberTable(output, docLookup, classes)
	if len(structs) > 0:
		output.write("<h2>Structures</h2>")
		MemberTable(output, docLookup, structs)
	if len(macros) > 0:
		output.write("<h2>Macros</h2>")
		MemberTable(output, docLookup, macros)
	if len(enums) > 0:
		output.write("<h2>Enumerations</h2>")
		MemberTable(output, docLookup, enums)
	if len(constants) > 0:
		output.write("<h2>Constants</h2>")
		MemberTable(output, docLookup, constants, showValue=True)
	if len(fields) > 0:
		output.write("<h2>Fields</h2>")
		MemberTable(output, docLookup, fields)
	if len(functions) > 0:
		output.write("<h2>Functions</h2>")
		MemberTable(output, docLookup, functions)
	if len(aliases) > 0:
		output.write("<h2>Aliases</h2>")
		MemberTable(output, docLookup, aliases, showValue=True)

def ShortDesc(desc):
	return desc[:(desc.index(".") + 1)]

def DocObjectFromDocField(parentDoc, field, isRef=False):
	args = field.split(" ")
	if len(args) >= 2:
		result = DocObject({
			"id": [args[0]],
			"fullid": [parentDoc.Info["fullid"][0] + "::" + args[0]],
			"name": [args[0]],
			"type": ["const"],
			"owner": [parentDoc.Info["fullid"][0]]
		}, args[0] + " = " + args[1])

		if len(args) > 2:
			desc = " ".join(args[2:])
			result.Info["description"] = [desc]
			result.Info["shortdesc"] = [ShortDesc(desc)]

		if isRef:
			result.Info["valueref"] = [args[1]]
		else:
			result.Info["value"] = [args[1]]
		
		return result

def ExportDocs(dir, name, docs, css, autoNamespace=None):
	docs = docs[:]
	groups = {}
	docLookup = {}

	docs.append(DocObject({
		"id": ["global"],
		"name": [name],
		"fullid": ["global"],
		"type": ["rootnamespace"]
	}, ""))

	for doc in docs:
		if not "type" in doc.Info:
			if doc.Declaration.startswith("namespace"):
				doc.Info["type"] = ["namespace"]
			elif doc.Declaration.startswith("class"):
				doc.Info["type"] = ["class"]
			elif doc.Declaration.startswith("struct") or doc.Declaration.startswith("typedef struct"):
				doc.Info["type"] = ["struct"]
			elif doc.Declaration.startswith("typedef"):
				doc.Info["type"] = ["alias"]
			elif doc.Declaration.startswith("enum"):
				doc.Info["type"] = ["enum"]
			elif doc.Declaration.startswith("const") and "=" in doc.Declaration:
				doc.Info["type"] = ["const"]
			elif doc.Declaration.startswith("#define"):
				doc.Info["type"] = ["macro"]
			elif doc.Declaration.startswith("virtual"):
				doc.Info["type"] = ["function"]
			elif doc.Declaration.endswith(";"):
				doc.Info["type"] = ["field"]
			else:
				doc.Info["type"] = ["function"]

		if autoNamespace != None and doc.Info["type"][0] != "namespace" and not "namespace" in doc.Info:
			doc.Info["namespace"] = [autoNamespace]

		if doc.Info["type"][0] == "class":
			name = GetClassName(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
		elif doc.Info["type"][0] == "struct":
			name = GetStructName(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
		elif doc.Info["type"][0] == "alias":
			name, value = GetAliasNameValue(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
			if len(value) > 0 and not "value" in doc.Info: doc.Info["value"] = [value]
		elif doc.Info["type"][0] == "macro":
			name, value = GetMacroNameValue(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
			if len(value) > 0 and not "value" in doc.Info: doc.Info["value"] = [value]
		elif doc.Info["type"][0] == "enum":
			name = GetEnumName(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
		elif doc.Info["type"][0] == "const":
			name, typeref, value = GetConstNameTyperefValue(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
			if len(typeref) > 0 and not "typeref" in doc.Info: doc.Info["typeref"] = [typeref]
			if len(value) > 0 and not "value" in doc.Info: doc.Info["value"] = [value]
		elif doc.Info["type"][0] == "field":
			name, typeref = GetFieldNameTyperef(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
			if len(typeref) > 0 and not "typeref" in doc.Info: doc.Info["typeref"] = [typeref]
		elif doc.Info["type"][0] == "function":
			name, typeref = GetFunctionNameTyperef(doc.Declaration)
			if len(name) > 0 and not "id" in doc.Info: doc.Info["id"] = [name]
			if len(typeref) > 0 and not "typeref" in doc.Info: doc.Info["typeref"] = [typeref]
		elif doc.Info["type"][0] == "ctor" and "class" in doc.Info and not "id" in doc.Info:
			name = doc.Info["class"][0]
			doc.Info["id"] = [name + "Ctor"]
			doc.Info["name"] = [name + " Constructor"]

		if "id" in doc.Info:
			if not "name" in doc.Info: doc.Info["name"] = [doc.Info["id"][0]]
			
			if not "shortdesc" in doc.Info and "description" in doc.Info:
				doc.Info["shortdesc"] = [ShortDesc(doc.Info["description"][0])]
			
			if doc.Info["type"][0] == "namespace" and not "namespace" in doc.Info: doc.Info["owner"] = ["global"]

			if not "fullid" in doc.Info:
				doc.Info["fullid"] = [doc.Info["id"][0]]
				if "class" in doc.Info: doc.Info["fullid"] = [doc.Info["class"][0] + "::" + doc.Info["fullid"][0]]
				if "namespace" in doc.Info: doc.Info["fullid"] = [doc.Info["namespace"][0] + "::" + doc.Info["fullid"][0]]

			baseId = doc.Info["fullid"][0]
			idIndex = 1
			while doc.Info["fullid"][0] in docLookup:
				doc.Info["fullid"][0] = baseId + str(idIndex)
				idIndex += 1

			if not "owner" in doc.Info:
				owner = ""
				if "class" in doc.Info and "namespace" in doc.Info:
					owner = doc.Info["namespace"][0] + "::" + doc.Info["class"][0]
				elif "namespace" in doc.Info:
					owner = doc.Info["namespace"][0]
				if len(owner) > 0: doc.Info["owner"] = [owner]

				if doc.Info["type"][0] == "ctor":
					index = str(docLookup[owner].Ctors)
					doc.Info["id"][0] += index
					doc.Info["fullid"][0] += index
					docLookup[owner].Ctors += 1

			if doc.Info["type"][0] == "enum" and ("field" in doc.Info or "fieldref" in doc.Info):
				members = []
				fields = []
				reffields = []

				if "field" in doc.Info: fields = doc.Info["field"]
				if "fieldref" in doc.Info: reffields = doc.Info["fieldref"]

				for field in fields: members.append(DocObjectFromDocField(doc, field))
				for field in reffields: members.append(DocObjectFromDocField(doc, field, True))

				docs += members
			
			groups[doc.Info["fullid"][0]] = []
			docLookup[doc.Info["fullid"][0]] = doc

	for doc in docs:
		if "owner" in doc.Info: groups[doc.Info["owner"][0]].append(doc)

	for doc in docs:
		if "id" in doc.Info:
			output = open(f"{dir}/{FileName(doc.Info)}", "w")
			output.write(f"<html><head><title>{HTML(doc.Info['name'][0])} - URCL</title><style>{css}</style></head><body>")

			output.write(f"<h1>{HTML(doc.Info['name'][0])}</h1>")
			if "namespace" in doc.Info and doc.Info["namespace"][0] in docLookup:
				output.write("<div><sub>Namespace: ")
				Link(output, docLookup[doc.Info["namespace"][0]].Info)
				output.write("</sub></div>")
			if "inherits" in doc.Info:
				for parent in doc.Info["inherits"]:
					if parent in docLookup:
						output.write("<div><sub>Inherits: ")
						Link(output, docLookup[parent].Info)
						output.write("</sub></div>")
			if "class" in doc.Info and "owner" in doc.Info and doc.Info["owner"][0] in docLookup:
				parent = docLookup[doc.Info["owner"][0]]
				output.write("<div><sub>")
				if parent.Info["type"][0] == "class": output.write("Class")
				elif parent.Info["type"][0] == "struct": output.write("Structure")
				elif parent.Info["type"][0] == "enum": output.write("Enumeration")
				output.write(": ")
				Link(output, parent.Info)
				output.write("</sub></div>")
			if len(doc.Declaration) > 0: output.write(f"<h2>Declaration</h2><code>{HTML(doc.Declaration)}</code>")
			AddIf(output, doc.Info, "description", "<h2>Description</h2><p>@info</p>")

			children = groups[doc.Info["fullid"][0]]
			if len(children) > 0: MemberContent(output, docLookup, children)
			
			output.write("</body><html>")
			output.close()

def PreprocessC(csource, cdestination, name, css):
	rawLines = csource.readlines()
	docs = []

	docLines = []
	nextLineIsDoc = False

	for rawLine in rawLines:
		line = rawLine.strip()

		if not line.startswith(DOC_PREFIX):
			cdestination.write(rawLine)
		
		if line.startswith(DOC_PREFIX):
			docLines.append(line)
			nextLineIsDoc = True
		elif nextLineIsDoc:
			docLines.append(line)
			docs.append(ParseDocComment(docLines))
			docLines.clear()
			nextLineIsDoc = False
	
	ExportDocs("./release/c/docs", name, docs, css, "urcl.h")
	return rawLines, docs

def InjectCIntoCPPAndPreprocess(clines, cdocs, cppsource, cppdestination, name, css):
	docs = cdocs

	docLines = []
	nextLineIsDoc = False

	for rawLine in cppsource.readlines():
		line = rawLine.strip()

		if line.lower() == "#include \"urcl.h\"":
			for cRawLine in clines:
				if not cRawLine.strip().startswith(DOC_PREFIX):
					cppdestination.write(f"\t{cRawLine}")
			cppdestination.write("\n")
		elif not line.startswith(DOC_PREFIX):
			cppdestination.write(rawLine)
		
		if line.startswith(DOC_PREFIX):
			docLines.append(line)
			nextLineIsDoc = True
		elif nextLineIsDoc:
			docLines.append(line)
			docs.append(ParseDocComment(docLines))
			docLines.clear()
			nextLineIsDoc = False
	
	ExportDocs("./release/cpp/docs", name, docs, css)

if not os.path.isdir("./src"):
	print("Source directory not found! Make sure you run this script from the project root directory.")
	exit(1)

os.makedirs("./release/c/docs", exist_ok=True)
os.makedirs("./release/cpp/docs", exist_ok=True)
os.makedirs("./release/python/docs", exist_ok=True)

name = "URCL Parser"
doccss = open("./src/docs.css", "r")
css = doccss.read().replace("\n", "")
doccss.close()

csource = open("./src/urcl.h", "r")
cppext = open("./src/urcl.hpp", "r")

c = open("./release/c/urcl.h", "w", newline='\n')
cpp = open("./release/cpp/urcl.hpp", "w", newline='\n')

clines, cdocs = PreprocessC(csource, c, name, css)
InjectCIntoCPPAndPreprocess(clines, cdocs, cppext, cpp, name, css)

csource.close()
cppext.close()

c.close()
cpp.close()