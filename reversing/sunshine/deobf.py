import os, fnmatch, string, re

def get_smali_files( path ):
	smali_files = []
	for root, dirnames, filenames in os.walk(path):
	    for filename in fnmatch.filter(filenames, '*.smali'):
	        smali_files.append(os.path.join(root, filename))
	return smali_files

def replace_file( files, old, new ):
	for i, f in enumerate(files):
		if f == old:
			files[i] = new
	return files

def read_file( name ):
	fd = open( name, 'rb' )
	data = fd.read()
	fd.close()
	return data

def write_file( name, data ):
	fd = open( name, 'wb' )
	fd.write( data )
	fd.close()

def patch_class_xrefs( files, classname, newname ):
	xrefs = 0
	for smali_file in files:
		data = read_file(smali_file)
		if classname in data:
			xrefs += 1
			data = data.replace( classname, newname )
			write_file( smali_file, data )
	return xrefs

def patch_field_xrefs( files, fieldname, newname ):
	xrefs = 0
	for smali_file in files:
		data = read_file(smali_file)
		if fieldname in data:
			xrefs += 1
			data = data.replace( fieldname, newname )
			write_file( smali_file, data )
	return xrefs

def parse_classname( filename ):
	data = read_file(filename)
	matches = re.search('^\.class .*(L.+;)', data)
	if matches:
		return matches.group(1)
	return None

def parse_fields( filename ):
	fields = []
	data = read_file(filename)
	for line in data.split("\n"):
		matches = re.search('^\.field.*\s(.+):.+;$', line)
		if matches:
			fields.append( matches.group(1) )

	return fields

base = "SunShine-latest"
# sono una brutta persona
os.system( "rm -rf %s" % base )
os.system( "apktool -r d %s.apk" % base)
path = "./%s/" % base

print "@ Processing class names ..."

smali_files = get_smali_files(path)

for i, smali_file in enumerate(smali_files):
	printable = all(c in string.printable for c in smali_file)
	if not printable:
		classname = parse_classname( smali_file )
		if classname is not None:
			name    = "Class%d" % i
			newname = re.sub('/[^/]+;$', "/%s;" % name, classname )

			dirname 	= os.path.dirname(smali_file)
			newfilename = os.path.join( dirname, "%s.smali" % name )

			print "@ Generated %s, patching xrefs ..." % newfilename

			n = patch_class_xrefs( smali_files, classname, newname )
			if n > 0:
				print "  %d xrefs patched." % n

			os.rename( smali_file, newfilename )
			# update files list after renaming
			smali_files = replace_file( smali_files, smali_file, newfilename )

print "@ Processing fields ..."

smali_files = get_smali_files(path)
field_idx = 0

for smali_file in smali_files:
	fields = parse_fields(smali_file)
	for field in fields:
		printable = all(c in string.printable for c in field)
		if not printable:
			newname = "field%d" % field_idx
			field_idx += 1
			xrefs = patch_field_xrefs( smali_files, field, newname )
			print "@ Patched field to %s with %d xrefs." % ( newname, xrefs )
