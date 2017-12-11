from os import listdir
from shutil import copyfile
from distutils.dir_util import copy_tree
import os.path
import os
import sys
from os.path import isfile, join, basename



print("Usage: python generateWidgetManualEntries.py ./path_to_widget_md_files")

if len(sys.argv) != 2:
	print("You must supply a directory")
	exit(1)

currentDir = os.path.abspath(sys.argv[1])
docfiles = listdir(currentDir)
os.chdir(currentDir)

if os.path.exists('./GeneratedWidgetFiles') == False:
	os.mkdir("GeneratedWidgetFiles")

for filename in docfiles:
    if ".md" in filename:
        #print(filename)
        links = []
        identifierCode = []
        #outputFile = open("_"+filename, 'w+')
        #outputFile = open(inputFile.name, 'w')
        #lineText = ""
        inputFile = open(filename)
        print("Widget Properties for "+filename)
        for line in inputFile:
            if "{! ./markdown/Widgets/Properties/" in line:
                line = line.replace("{! ./markdown/Widgets/Properties/", "")
                line = line[0:line.find(" !}")]
                #print(line)
                propFile = open("./Properties/"+line.rstrip())
                content = propFile.readlines()
                for propLine in content:
                    if "**" in propLine:
                        propLine = propLine[propLine.find("**")+2:-1]
                        params = propLine[propLine.find("("):propLine.find(")")+1]
                        propLine = propLine[0:propLine.find("(")]
                        linkText = "["+propLine+"](#" + line[0:line.find(".")] +")" + params +", "
                        #print (linkText)

                        links.append(linkText + "\n")
                    
                    if "<!--UPDATE WIDGET_IN_CSOUND" in propLine:
                        lineIndex = content.index(propLine)+1
                        while lineIndex < len(content)-1:
                            identifierCode.append(content[lineIndex])
                            lineIndex+=1
        
        inputFile = open(filename)
        htmlText = ""
        for line in inputFile:
            if "WIDGET_SYNTAX" in line:
                line = line.replace("WIDGET_SYNTAX", ''.join(links))
        
            htmlText = htmlText+line;

            if ";WIDGET_ADVANCED_USAGE" in line:
                newLines = '''
                instr 2
                    kUpdate init 0
                    iCnt init 0
                    if changed:k(chnget:k("changeAttributes")) == 1 then
                        event "i", "ChangeAttributes", 0, 1, kUpdate
                        kUpdate = (kUpdate < 6 ? kUpdate+1 : 0)
                    endif
                endin

                instr ChangeAttributes
                    SIdentifier init ""\n'''  
                newLines +=''.join(identifierCode)      

                lastBit ='''
                    chnset SIdentifier, "buttonIdent"     
                    prints SIdentifier         
                endin'''
                newLines+=lastBit

                line = line.replace(";WIDGET_ADVANCED_USAGE", newLines)
                htmlText = htmlText+line;

        inputFile = open("./GeneratedWidgetFiles/"+filename, "w+")
        inputFile.write(htmlText)
        inputFile.close();

#print htmlText
    #         <big><pre>
    # [bounds](#bounds)(x, y, width, height) 
    # [channel](#channel)("chan") 
    # [text](#text)("offCaption","onCaption")
    # [bounds](#bounds)(x, y, width, height) 
    # [channel](#channel)("chan") 
    # [text](#text)("offCaption","onCaption") 
    # [bounds](#bounds)(x, y, width, height) 
    # [channel](#channel)("chan") 
    # [text](#text)("offCaption","onCaption")
    # </pre></big>

        #     outputFile.write(line)

        # inputFile.close()
        # outputFile.close()

