import os

def listfiles( path ):
    l = os.listdir( path )
    res = []
    for f in l:
        res.append( os.path.normpath( os.path.join( path, f ) ) )
    return res

def count_lines( fnm ):
    "Given a filename 'fnm', return the number of lines in the file"
    f = open( fnm, 'r' )
    lines = f.readlines()
    f.close()
    return len( lines )

def count_total_lines( fl ):
    res = 0
    for fnm in fl:
        res = res + count_lines( fnm )
    return res

def endswith( str, suffix ):
    return str[-len( suffix ):] == suffix

def classify_file( fnm ):
    if( fnm == '' ):
        return "unknown"
    if( os.path.isdir( fnm ) ):
        return "directory"
    (root,ext) = os.path.splitext( fnm )
    if( ext == '.cct' or ext == '.ct'  or ext == '.ht' or ext == '.t'):
        return "Tm"
    if( endswith( fnm, '-g.cc' ) ):
        return "Tm generated"
    if ext == '.y':
        return "yacc"
    if fnm in ['Makefile', '.cvsignore', 'Makefile.in']:
        return 'irrelevant'
    if( ext in ['.o', '.v', '.java', '.spar', '.s', '.py', '.txt', '.class', '.ds', '.lnt', '.ac', '.a'] ):
        return "irrelevant"
    if( ext == '.cc' ):
        return "handwritten"
    if( ext == '.c'  or ext == '.h' ):
	if os.path.exists( fnm + 't' ):
	    return "Tm generated"
	if (ext == '.c' and os.path.exists( root + '.y' ) ):
	    return "yacc generated"
        return "handwritten"
    return "unknown"

def is_analyzer_file( fnm ):
    ( path, file ) = os.path.split( fnm ) 
    return file == 'codesize.c' or file == 'codesize.ct' or file[0:2] == 'is'

def report_stat( str, val, total ):
    if val != 0:
	perc = 100.0*val/total
        print "%s\t%d\t%3.1f" % ( str, val, perc )

handwritten = []
tm_admin_source = []
tm_admin_generated = []
tm_analyzer_source = []
tm_analyzer_generated = []
tm_source = []
tm_generated = []
yacc_source = []
yacc_generated = []
unknown = []

files = listfiles( '.' )
#print files

for file in files:
    t = classify_file( file )
    #print "%s -> %s" % ( file, t )
    if( t != "directory" and t != 'hidden'):
	if( endswith( file, 'tmadmin.ct' ) or endswith( file, 'tmadmin.ht' ) ):
	    tm_admin_source.append( file )
	if( endswith( file, 'tmadmin.c' ) or endswith( file, 'tmadmin.cc' ) or endswith( file, 'tmadmin.h' ) ):
	    tm_admin_generated.append( file )
	elif( t == "handwritten" ):
	    handwritten.append( file )
	elif( t == "yacc" ):
	    yacc_source.append( file )
	elif( t == "yacc generated" ):
	    yacc_generated.append( file )
	elif( t == "Tm" ):
	    if( is_analyzer_file( file ) ):
		tm_analyzer_source.append( file )
	    else:
		tm_source.append( file )
	elif( t == "Tm generated" ):
	    if( is_analyzer_file( file ) ):
		tm_analyzer_generated.append( file )
	    else:
		tm_generated.append( file )
	elif( t == "unknown" ):
	    unknown.append( file )
handwritten_count = (count_total_lines( handwritten ) + count_total_lines( tm_source ) + count_total_lines( tm_analyzer_source ) + count_total_lines( tm_admin_source ) + count_total_lines( yacc_source ))
administration_count = (count_total_lines( tm_admin_generated ) - count_total_lines( tm_admin_source ))
treewalker_count = (count_total_lines( tm_generated ) - count_total_lines( tm_source ))
analyzer_count = (count_total_lines( tm_analyzer_generated ) - count_total_lines( tm_analyzer_source ))
yacc_count = (count_total_lines( yacc_generated ) - count_total_lines( yacc_source ))
total_count = handwritten_count + administration_count + treewalker_count + analyzer_count + yacc_count
report_stat( "handwritten", handwritten_count, total_count )
report_stat( "Tm administration", administration_count, total_count )
report_stat( "Tm treewalkers", treewalker_count, total_count )
report_stat( "Tm analyzers", analyzer_count, total_count )
report_stat( "yacc", yacc_count, total_count )
#print "handwritten:", handwritten
#print "tm admin source:", tm_admin_source
#print "tm admin generated:", tm_admin_generated
#print "tm analyzer source:", tm_analyzer_source
#print "tm analyzer generated:", tm_analyzer_generated
#print "tm source:", tm_source
#print "tm generated:", tm_generated
#print "yacc source:", yacc_source
#print "yacc generated:", yacc_generated
#print "unknown", unknown
