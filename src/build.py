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

def ExportDocs(dir, docs):
	groups = {}

	for doc in docs:
		if "id" in doc.Info:
			if not "name" in doc.Info: doc.Info["name"] = [doc.Info["id"][0]]
			if not "fullid" in doc.Info: doc.Info["fullid"] = [doc.Info["id"][0]]
			if "class" in doc.Info: doc.Info["fullid"] = [doc.Info["class"][0] + "::" + doc.Info["fullid"][0]]
			if "namespace" in doc.Info: doc.Info["fullid"] = [doc.Info["namespace"][0] + "::" + doc.Info["fullid"][0]]

	for doc in docs:
		if "id" in doc.Info:
			if "class" in doc.Info: groups[doc.Info["class"]] = doc.Info

			output = open(f"{dir}/{doc.Info['id'][0]}.html", "w")
			output.write(f"<html><head>")
			
			output.write(f"<title>{HTML(doc.Info['name'][0])} - URCL</title>")
			output.write("<style>body { color: #EEEEEE; background-color: #171717; font-family: Arial, Helvetica, sans-serif; margin-top: 2%; margin-bottom: 2%; margin-left: 10%; margin-right: 10%; } h1 { margin-bottom: 0.1em; } sub { color: #AAAAAA; } code { color: #DDDDDD; border: 1px solid #333333; border-radius: 5px; padding: 10px; }</style>")
			
			output.write("</head><body>")

			output.write(f"<h1>{HTML(doc.Info['name'][0])}</h1>")
			AddIf(output, doc.Info, "namespace", "<sub>namespace @info</sub>")
			AddIf(output, doc.Info, "class", "<sub>class @info</sub>")
			output.write(f"<h2>Declaration</h2><code>{HTML(doc.Declaration)}</code>")
			AddIf(output, doc.Info, "description", "<p>@info</p>")
			
			output.write("</body><html>")
			output.close()

def PreprocessC(csource, cdestination):
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
	
	ExportDocs("./release/c/docs", docs)
	return rawLines

def InjectCIntoCPPAndPreprocess(clines, cppsource, cppdestination):
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
	
	ExportDocs("./release/cpp/docs", docs)

if not os.path.isdir("./src"):
	print("Source directory not found! Make sure you run this script from the project root directory.")
	exit(1)

os.makedirs("./release/c/docs", exist_ok=True)
os.makedirs("./release/cpp/docs", exist_ok=True)
os.makedirs("./release/python/docs", exist_ok=True)

csource = open("./src/urcl.h", "r")
cppext = open("./src/urcl.hpp", "r")

c = open("./release/c/urcl.h", "w", newline='\n')
cpp = open("./release/cpp/urcl.hpp", "w", newline='\n')

clines = PreprocessC(csource, c)
InjectCIntoCPPAndPreprocess(clines, cppext, cpp)

csource.close()
cppext.close()

c.close()
cpp.close()