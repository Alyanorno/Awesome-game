import re
from datetime import date

log = open('log.log', 'r')
text = []
for line in log:
	if line.startswith('-'): continue
	text.append( line )
log.close()

out_text = []
out_line = ''
added = 0
removed = 0
for line in text:
	if line[:2] == '20':
		if out_line == '':
			out_line = line[:10]
	elif line == '\n' or line == '':
		out_text.append( out_line + '\t' + str(added) + '\t' + str(removed) + '\t' + str(added+removed) + '\n' )
		out_line = ''
		added = 0
		removed = 0
		total = 0
	else:
		n1, n2 = re.findall( '\d+', line )
		added += int(n1)
		removed -= int(n2)
out_text.append( out_line + '\t' + str(added) + '\t' + str(removed) + '\t' + str(added+removed) + '\n' )
out_line = ''
added = 0
removed = 0
total = 0

week_text = []
week = 0
first = True
total = 0
for line in out_text:
	if first == True:
		i = re.findall( '\d+', line )
		d = date( int(i[0]), int(i[1]), int(i[2]) )
		week = d.isocalendar()[1]
		added = int(i[3])
		removed = -int(i[4])
		first = False
	else:
		i = re.findall( '\d+', line )
		d = date( int(i[0]), int(i[1]), int(i[2]) )
		w = d.isocalendar()[1]
		if w == week:
			added += int(i[3])
			removed -= int(i[4])
		else:
			total += added + removed;
			week_text.append( str(week) + '\t' + str(added) + '\t' + str(removed) + '\t' + str(total) + '\n' )
			if w - week > 1:
				print( "lost months\n" )
				for t in range( week + 1, w ):
					week_text.append( str(t) + "\t0\t0\t" + str(total) + "\n" )
					print( "adding lost month " + str(t) + "\n" )
			week = w;
			added = int(i[3])
			removed = -int(i[4])
total += added + removed;
week_text.append( str(week) + '\t' + str(added) + '\t' + str(removed) + '\t' + str(total) + '\n' )


out = open('graph.txt', 'w')
for line in week_text:
	out.write( line )
out.close()

