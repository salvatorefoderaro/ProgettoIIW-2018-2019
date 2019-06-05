import xlsxwriter
import sys

workbook = xlsxwriter.Workbook(sys.argv[1] + ".xlsx")
worksheet = workbook.add_worksheet()

riga = 0
colonna = 0
inFile = open(sys.argv[1])

for line in inFile:
    
    if line.startswith("Richieste in contemporanea: "):
        if riga == 0:
            toWrite = line[line.index("Richieste in contemporanea: ") + len("Richieste in contemporanea: "):].replace('s', '')
            toWrite = toWrite.rstrip()
            worksheet.write(riga, colonna, float(toWrite))

    if line.startswith("Time taken to complete requests: "):
        toWrite = line[line.index("Time taken to complete requests: ") + len("Time taken to complete requests: "):].replace('s', '')
        toWrite = toWrite.rstrip()
        toWrite = toWrite.replace(" ", "")
        if ("m" in toWrite and toWrite.index("m") == (len(toWrite) -1)):
           toWrite = toWrite.replace("m", "")
           toWrite = toWrite.replace(".", "")
           toWrite = "0." + str(int(toWrite))
        elif ("m" in toWrite):
            toWrite = int(toWrite[:toWrite.index("m")])*60 + float(toWrite[toWrite.index("m")+1:])
        worksheet.write(riga+2, colonna, float(toWrite))

    if line.startswith("Max response time (ms): "):
        toWrite = line[line.index("Max response time (ms): ") + len("Max response time (ms): "):].replace('s', '')
        toWrite = toWrite.rstrip()
        toWrite = toWrite.replace(" ", "")
        worksheet.write(riga+7, colonna, float(toWrite))

    if line.startswith("Avg response time (ms): "):
        toWrite = line[line.index("Avg response time (ms): ") + len("Avg response time (ms): "):].replace('s', '')
        toWrite = toWrite.rstrip()
        toWrite = toWrite.replace(" ", "")
        worksheet.write(riga+12, colonna, float(toWrite))
        riga = riga + 1

    if line.startswith("Requests per second: "):
        toWrite = line[line.index("Requests per second: ") + len("Requests per second: "):]
        toWrite = toWrite.rstrip()
        toWrite = toWrite.replace(" ", "")
        worksheet.write(riga+17, colonna, int(toWrite))
    
    if line.startswith("Apache"):
        colonna = colonna + 1

    if (riga == 3):
        colonna = colonna + 1
        riga = 0

inFile.close()
workbook.close()
