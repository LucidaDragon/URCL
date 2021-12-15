import os

DOC_PREFIX = "////"

class DocObject:
	def __init__(self, info, decl):
		self.Info = info
		self.Declaration = decl

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

def Link(output, info):
	output.write(f"<a href=\"./{info['id'][0]}.html\">{HTML(info['name'][0])}</a>")

def MemberTable(output, members):
	members = sorted(members, key=lambda member: member["name"][0])

	output.write("<table>")
	for member in members:
		output.write("<tr><td>")
		Link(output, member)
		output.write("</td><td>")
		AddIf(output, member, "shortdesc", "<p>@info</p>")
		output.write("</td></tr>")
	output.write("</table>")

def ExportDocs(dir, docs, css):
	groups = {}

	for doc in docs:
		if "id" in doc.Info:
			if not "name" in doc.Info: doc.Info["name"] = [doc.Info["id"][0]]
			if not "shortdesc" in doc.Info and "description" in doc.Info:
				firstSentenceLength = doc.Info["description"][0].index(".") + 1
				doc.Info["shortdesc"] = [doc.Info["description"][0][:firstSentenceLength]]
			if not "fullid" in doc.Info: doc.Info["fullid"] = [doc.Info["id"][0]]
			if "class" in doc.Info: doc.Info["fullid"] = [doc.Info["class"][0] + "::" + doc.Info["fullid"][0]]
			if "namespace" in doc.Info: doc.Info["fullid"] = [doc.Info["namespace"][0] + "::" + doc.Info["fullid"][0]]
			groups[doc.Info["fullid"][0]] = []

	for doc in docs:
		owner = ""
		if "class" in doc.Info and "namespace" in doc.Info:
			owner = doc.Info["namespace"][0] + "::" + doc.Info["class"][0]
		if "namespace" in doc.Info:
			owner = doc.Info["namespace"][0]
		if len(owner) > 0:
			groups[owner].append(doc)

	for doc in docs:
		if "id" in doc.Info:
			output = open(f"{dir}/{doc.Info['id'][0]}.html", "w")
			output.write(f"<html><head><title>{HTML(doc.Info['name'][0])} - URCL</title><style>{css}</style></head><body>")

			output.write(f"<h1>{HTML(doc.Info['name'][0])}</h1>")
			AddIf(output, doc.Info, "namespace", "<sub>namespace @info</sub>")
			AddIf(output, doc.Info, "class", "<sub>class @info</sub>")
			output.write(f"<h2>Declaration</h2><code>{HTML(doc.Declaration)}</code>")
			AddIf(output, doc.Info, "description", "<h2>Description</h2><p>@info</p>")

			children = groups[doc.Info["fullid"][0]]
			if doc.Declaration.startswith("namespace") and len(children) > 0:
				classes = []
				structs = []
				enums = []
				functions = []

				for member in children:
					if member.Declaration.startswith("class"):
						classes.append(member.Info)
					elif member.Declaration.startswith("struct"):
						structs.append(member.Info)
					elif member.Declaration.startswith("enum"):
						enums.append(member.Info)
					else:
						functions.append(member.Info)

				if len(classes) > 0:
					output.write("<h2>Classes</h2>")
					MemberTable(output, classes)
				if len(structs) > 0:
					output.write("<h2>Structures</h2>")
					MemberTable(output, structs)
				if len(enums) > 0:
					output.write("<h2>Enumerations</h2>")
					MemberTable(output, enums)
				if len(functions) > 0:
					output.write("<h2>Functions</h2>")
					MemberTable(output, functions)
			
			output.write("</body><html>")
			output.close()

def PreprocessC(csource, cdestination, css):
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
	
	ExportDocs("./release/c/docs", docs, css)
	return rawLines

def InjectCIntoCPPAndPreprocess(clines, cppsource, cppdestination, css):
	docs = []

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
	
	ExportDocs("./release/cpp/docs", docs, css)

if not os.path.isdir("./src"):
	print("Source directory not found! Make sure you run this script from the project root directory.")
	exit(1)

os.makedirs("./release/c/docs", exist_ok=True)
os.makedirs("./release/cpp/docs", exist_ok=True)
os.makedirs("./release/python/docs", exist_ok=True)

doccss = open("./src/docs.css", "r")
css = doccss.read().replace("\n", "")
doccss.close()

csource = open("./src/urcl.h", "r")
cppext = open("./src/urcl.hpp", "r")

c = open("./release/c/urcl.h", "w", newline='\n')
cpp = open("./release/cpp/urcl.hpp", "w", newline='\n')

clines = PreprocessC(csource, c, css)
InjectCIntoCPPAndPreprocess(clines, cppext, cpp, css)

csource.close()
cppext.close()

c.close()
cpp.close()