#!/usr/bin/env python
#--*-- encoding: utf8 --*--



from HTMLParser import HTMLParser
import sys
import tidy
import re
import codecs



CHARSET_PATTERN = re.compile(r'content="text/html;\s*charset=(\S+)\s*"',re.I)
DEFAULT_CHARSET = 'gb2312'

def detect_charset(content):
    match = CHARSET_PATTERN.search(content)
    if match:
        try:
            codecs.lookup(match.group(1))
            return match.group(1)
        except LookupError:
            pass
    return DEFAULT_CHARSET

class IRParser(HTMLParser):
    _title = []
    _copyright = ''
    _istitle = False


    def handle_starttag(self, tag, attrs):
        if tag =='title':
            self._istitle = True
        elif tag == 'meta':
            try:
                attrs = dict(attrs)
                if attrs['name'].lower() == 'copyright':
                    self._copyright = attrs['content']
            except KeyError:
                pass

    def handle_endtag(self, tag):
        if tag == 'title':
            self._istitle = False

       
    
    
    def handle_data(self,data):
        if self._istitle:
            self._title.append(data)
    
    def get_title(self):
        return ''.join(self._title)
    
    def get_copyright(self):
        return self._copyright
    
    def reset(self):
        self._title = []
        self._istitle = False
        self._copyright = ''
        HTMLParser.reset(self)

def main():
    p = IRParser()

    for arg in sys.argv[1:]:
        try:
            f = file(arg)
        except:
            sys.stderr.write('can not open file %s\n'%arg)
            continue
        content = f.read()
        f.close()
        content = content.decode(detect_charset(content),'ignore')\
            .encode('utf8')
        options = dict(char_encoding = 'utf8')
        content2 = str(tidy.parseString(content,**options))
        if len(content2):
            content = content2

        try:
            p.feed(content)
        except:
            pass

        if len(p.get_title()):
            print p.get_title()
        if len(p.get_copyright()):
            print p.get_copyright()
        
    
        p.reset()

    p.close()



if __name__ == '__main__':
    main()
