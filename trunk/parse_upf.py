from BeautifulSoup import BeautifulSoup
import sys
import pprint
""" A really hackish little UPF parser
"""

def expand_path(vars,path,passes=3):
    """ expands a path using the variables provided
        does multiple passes, not the correct way to do stuff, but works
    """
    for x in xrange(passes):
        for k,v in vars.items():
            path = path.replace('${%s}' % k,v)
    return path

def parse_upf_str(s):
    """ Parses a UPF file passed as a long string
    """
    soup = BeautifulSoup(s)
    try:
       retval = {}
       retval['proj_name'] = soup.contents[0]['name']
       retval['variables'] = {}
       for varset in soup.findAll('set'):
           retval['variables'][varset['name']] = varset['value']
       for varset in soup.findAll('set-default'):
           retval['variables'][varset['name']] = varset['value']
       retval['resources'] = {}
       retval['resources']['scripts'] = []
       retval['resources']['sounds']  = []
       retval['resources']['motions'] = []
       resources = soup.find('resources')
       vars = retval['variables']
       for script in resources.findAll('script'):
           retval['resources']['scripts'].append(expand_path(vars,script['path']))
       for sound in resources.findAll('sound'):
           retval['resources']['sounds'].append(expand_path(vars,sound['path']))
       for motion in resources.findAll('motion'):
           retval['resources']['motions'].append(expand_path(vars,motion['path']))
 
    except Exception,e:
       print e
       raise Exception('Parse error')
    return retval

def parse_upf_file(filename):
    """ Parses a UPF file
    """
    f = open(filename,'r')
    data = f.read()
    f.close()
    return parse_upf_str(data)

if __name__ == '__main__':
   pprint.pprint(parse_upf_file(sys.argv[1]))
