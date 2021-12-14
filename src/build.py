import os

def InjectCIntoCPP(csource, cppsource, cdestination, cppdestination):
	for rawLine in cppsource.readlines():
		if rawLine.strip().lower() == "#include \"urcl.h\"":
			for cRawLine in csource.readlines():
				cdestination.write(cRawLine)
				cppdestination.write(f"\t{cRawLine}")
		else:
			cppdestination.write(rawLine)

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

InjectCIntoCPP(csource, cppext, c, cpp)

csource.close()
cppext.close()

c.close()
cpp.close()