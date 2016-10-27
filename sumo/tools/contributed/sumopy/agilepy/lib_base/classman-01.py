
# python classman.py


# new structure
##obj0
##|
##|-attr1
##|-attr2
##|
##|-tab1
##|  |
##|  |-attr1
##|  |-attr2
##|  |
##|  |-arraycol1
##|  |  |
##|  |  |-1:x1
##|  |  |-2:x2
##|  |
##|  |-childcol1
##|  |  |
##|  |  |-1:obj1
##|  |  |-2:obj2
##|  |
##|  |
##|  |-tabchildcol1(tab2)
##|  |  |
##|  |  |-1:id1
##|  |  |-2:id2 
##|
##|
##|
##|
##|-child1[obj.child1]
##|   |
##|   |- attr1
##
##IDEA : make childof column
##tab2
##|
##|-arraycol1
##|  |-id1:x1
##|  |-id2:x2
##|
##|
##|-arraycol2
##|  |-id1:y1
##|  |-id2:y2

## To be or not to be.  -- Shakespeare 
## To do is to be.  -- Nietzsche 
## To be is to do.  -- Sartre 
## Do be do be do.  -- Sinatra 

# save with is saved flag
# xml mixin
# different attrconfig classe (numbers, strings, lists, colors,...)

import types, os, pickle, sys, string
#import numpy as np
import xmlmanager as xm
from datetime import datetime

# oh my god....
import platform
global IS_WIN
if platform.system()=='Windows':
    IS_WIN = True
else:
    IS_WIN = False
 
# event triggers
#       plugtype    plugcode
EVTDEL =  0 # delete attribute 
EVTSET =  1 # set attribute
EVTGET =  2 # get attribute
EVTADD =  3 # add/create attribute

##EVTDELITEM = 20 # delete attribute 
##EVTSETITEM = 21 # set attribute
##EVTGETITEM = 22 # get attribute
##EVTADDITEM = 23 # add/create attribute

ATTRS_NOSAVE = ('plugin','_obj','_manager')
      
ATTRS_SAVE = ['ident','name','managertype',]         
        

def filepathlist_to_filepathstring(filepathlist, sep=',',is_primed = False):
    if IS_WIN & is_primed:
        p = '"'
    else:
        p=''
    #print 'filepathlist_to_filepathstring',IS_WIN,p,filepathlist
    if type(filepathlist)==types.ListType:
        if len(filepathlist) == 0:
            return ''
        else:
            filepathstring = ''
            for filepath in filepathlist[:-1]:
                fp = filepath.replace('"','')
                filepathstring += p+fp+p+sep
            filepathstring += p+filepathlist[-1]+p
            return filepathstring
    else:
        fp = filepathlist.replace('"','')
        return p+filepathlist+p

def filepathstring_to_filepathlist(filepathstring, sep=',', is_primed = False):
    if IS_WIN & is_primed:
        p='"'
    else:
        p=''
    filepaths=[]
    for filepath in filepathstring.split(sep):
        filepaths.append(p+filepath.strip().replace('"','')+p)
    return filepaths



def save_obj(obj,filename):
    """
    Saves python object to a file with filename.
    Filename may also include absolute or relative path.
    If operation fails a False is returned and True otherwise.
    """
    try:
        file=open(filename,'wb')
    except:
        print 'WARNING in save: could not open',simname
        return False
	
    #try:
    pickle.dump(obj, file, protocol=2)
    file.close()
    return True
    #except:
    #    file.close()
    #    print 'WARNING in save: could not pickle object'
    #    return False

def load_obj(filename,parent=None):
    """
    Reads python object from a file with filename and returns object.
    Filename may also include absolute or relative path.
    If operation fails a None object is returned.
    """
    print 'load_obj',filename
    try:
        file=open(filename,'rb')
    except:
        print 'WARNING in load_obj: could not open',filename
        return None
    
    #try:
    #print '  pickle.load...'
    obj=pickle.load(file)
    file.close()
    #print '  obj._link2'
    
    # _init2_ is to restore INTERNAL states from INTERNAL states
    obj._init2_config()
    
    # _init3_ is to restore INTERNAL states from EXTERNAL states 
    # such as linking
    obj._init3_config()
    
    # _init4_ is to do misc stuff when everything is set
    obj._init4_config()
    
    return obj
    
    #except:
	#	print 'WARNING in load: could not load object'
	#	return None	 
	
class Plugin:
    
    def _init__(self,obj):
        self._obj =  obj # this can be either attrconf or main object 
        self._events = {}
        self._has_events = False
    
    
     
    
    
        
    def add_event(self,trigger,function):
        """
        Standard plug types are automatically set but the system:

        """
        if not self._events.has_key(trigger):
            self._events[trigger] = []
        self._events[trigger].append(function) 
        self._has_events = True    

    def del_event(self,trigger):
        del self._events[trigger]
        if len(self._events)==0:
            self._has_events = False
    
    
    
    
        
    def exec_events(self,trigger):
        if self._has_events: 
            #print '**PuginMixin.exec_events',trigger,(EVTGETITEM,EVTGET)
            #if trigger!=EVTGET:
            #    print '  call set_modified',self._obj
            #    self._obj.set_modified(True)
            
            for function in self._events.get(trigger,[]): 
                function(self._obj)
        

  

class AttrConf:
    """
    Contains additional information on the object's attribute.
    """
    def __init__(self, attrname, default = None,
                    groupnames = [], perm='rw', 
                    is_save = True, 
                    #is_link = False, # define link class
                    is_copy = True,
                    name = '', info = '', 
                    unit = '',
                    is_plugin = False,
                    struct = 'scalar', # depricated, metatype is class
                    metatype = '', # depricated, metatype is class
                    **attrs):

        # these states will be saved and reloaded
        self.attrname = attrname
        self.groupnames = groupnames
        self.metatype = metatype
        self.struct = struct
        
        self._default = default
        
        
        self._is_save = is_save
        #self._is_link = is_link
        self._is_copy = is_copy
        
        self._unit = unit
        self._info = info
        self._name = name 
        self._perm = perm
        
        # states below need to be resored after load
        self._manager = None # set later by attrman , necessary?
        self._obj = None # parent object, set later by attrman
        
        self._is_modified = False
        self._is_saved = False
        
        if is_plugin:
            self.plugin = Plugin(self)
            self.set = self.set_plugin
            self.get = self.get_plugin
        else:
            self.plugin = None
            
        # set rest of attributes passed as keyword args
        # no matter what they are used for
        for attr, value in attrs.iteritems():
            setattr(self,attr,value)
        
    def get_name(self):
        return self._name
      
    def is_modified(self):
        return  self._is_modified
           
    def set_manager(self, manager):
        """
        Method to set manager to attribute configuration object.
        This is either attribute manager or table manager.
        Used by add method of AttrManager
        """
        self._manager = manager
        
     
        
    def get_manager(self):
        """
        Method to get manager to attribute configuration object.
        """
        return self._manager 
           
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        self._obj = obj  
    
    def get_obj(self):
        return self._obj
        
    def get_val(self):
        # always return attribute, no indexing, no plugin
        
        return getattr(self._obj, self.attrname)
    
    
    def set_attr(self, value):
        #set entire attribute, no indexing, no plugin
        
        return setattr(self._obj, self.attrname, value)    

    def get(self):
        #  return attribute, overridden with indexing for array and dict struct
        return getattr(self._obj, self.attrname)
      
    def set(self, value):
        # set attribute, overridden with indexing for array and dict struct
        setattr(self._obj, self.attrname, value)   
        self._is_modified = True
        return value 

    
    
    def get_plugin(self):
        """
        Default get method with plugin for scalar attrs
        """
        #  return attribute, overridden with indexing for array and dict struct
        self.exec_events(EVTGET)
            
        return getattr(self._obj, self.attrname)
      
    def set_plugin(self, value):
        """
        Default set method with plugin for scalar attrs
        """
        # set attribute, overridden with indexing for array and dict struct
        if value != getattr(self._obj, self.attrname):
            setattr(self._obj, self.attrname, value)
            self.exec_events(EVTSET)
            self._is_modified = True
        return value
    
    
    def get_default(self):
        return self._default
    
    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        return self.get_default()
    
    def is_tableattr(self):
        return self.struct in ('dict','array','list')
    
    def set_perm(self, perm):
        self._perm = perm
         
    def get_perm(self):
        return self._perm
    
    def is_readonly(self):
        return 'w' not in self._perm
     
    def is_writable(self):
        return 'w' in self._perm   
    
    def is_editable(self):
        """Can attribute be edited """
        return 'e' in self._perm
    
    #def is_save(self):
    #    return self._is_save
    
    def save_value(self, state):
        """
        Save attribute value of managed object.
        """
        if self._is_save:
            self._is_modified = False
            state[self.attrname]= self.get_val()
    
    def has_unit(self):
        return self._unit != ''
    
    
        
    def has_info(self):
        return self.get_info() != ''
    
    def get_info(self):
        return self.info
       
    def format_unit(self, show_parentesis=False):
        if self._unit in ('',None):
            return ''
        if show_parentesis:
            return '[%s]'%self._unit
        else:
            return '%s'%self._unit
        
    
    def format_value(self, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        return repr(self.get_val())+unit
     
 

        
    def  __getstate__(self):
        #print '__getstate__',self.ident
        #print '  self.__dict__=\n',self.__dict__.keys()
        
        state={} 
        for attr in self.__dict__.keys():
                if attr =='plugin':
                    plugin = self.__dict__[attr]
                    if plugin!=None:
                        state[attr] = True
                    else:
                        state[attr] = False
                    
                elif attr not in ATTRS_NOSAVE:
                    state[attr] = self.__dict__[attr]
                    
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        self._plugins={}
        
        for attr in state.keys():
            #print '  set state',key
            if attr=='plugin':
                if state[attr]==True:
                    self.__dict__[attr] = Plugin(self)
                else:
                    self.__dict__[attr]= None
            else:
                self.__dict__[attr]=state[attr]
                
    


class ChildConf(AttrConf):
    """
    Contains additional information on the object's attribute.
    This configures a child class.
    """
    
    def __init__(self, default, **kwargs):
        attrname = default.get_ident()
        AttrConf.__init__(self,  attrname, default, 
                                struct = 'scalar', 
                                metatype = 'child',
                                **kwargs
                                )
        
        
    def save_value(self, state):
        """
        Save attribute value of managed object.
        """
        if self._is_save:
            self._is_modified = False
            state[self.attrname]= self.get_val()
    

    def get_name(self):
        return self.get_value().get_name()
    
    def get_info(self):
        return self.get_value().__doc__
       
    def format_value(self, show_unit = False, show_parentesis=False):
        return repr(self.get_val())
     
    
class LinkConf(AttrConf):
    """
    Contains additional information on the object's attribute.
    This configures a child class.
    """
    
    def __init__(self, default, **kwargs):
        attrname = default.get_ident()
        AttrConf.__init__(self,  attrname, default, 
                                struct = 'scalar', 
                                metatype = 'link',
                                **kwargs
                                )
        
        
    def save_value(self, state):
        """
        Save attribute value of managed object.
        """
        if self._is_save:
            self._is_modified = False
            # this is the trick to avoid endless save-loops:
            # return the absolute ident and replace with
            # instance in init_external after load 
            state[self.attrname]= self.get_ident_abs()
    

    def get_name(self):
        return self.get_value().get_name()
    
    def get_info(self):
        return self.get_value().__doc__
       
    def format_value(self, show_unit = False, show_parentesis=False):
        return repr(self.get_val())
     
 
    
class AttrsManager:
    """
    Manages all attributes of an object
    """
    def __init__(self, obj, attrname = 'attrman', is_plugin = False):
        self._obj = obj # managed object
        self._attrconfigs = []  # managed attribute config instances
        self.attrname = attrname # the manager's attribute name in the obj instance
        # groupes of attributes 
        # key=groupname, value = list of attribute config instances
        self._groups = {} 
        if is_plugin:
            self._plugin = Plugin(obj)
        else:
            self._plugin = None
            
    #def exec_events(self,trigger, attrconfig):
    #    if self._has_events: 
    #        #if trigger!=EVTGET:
    #        #    print '  call set_modified',self._obj
    #        #    self._obj.set_modified(True)
    #        #print '**AttrsManager.exec_events NOMOD',attrconfig.attrname,trigger
    #        # overridden plugin default because we pass the attrconfig
     #       for function in self._events.get(trigger,[]): function(attrconfig)
    def init_internal(self, obj):
        """
        Called after set state.
        Link internal states.
        """
        self._obj = obj
        for attrconfig in  self.get_configs(is_all = True):
            attrconfig.set_manager(self)
            attrconfig.set_obj(obj)
            if attrconfig.metatype == 'child':
                attrconfig.get_val().init_internal()
    
    def init_external(self):
        """
        Called after set state.
        Link exiernal states.
        """
        print 'init_external',self._obj.get_ident()
        for attrconfig in  self.get_configs(is_all = True):
            if attrconfig.metatype == 'link':
                ident_abs = attrconfig.get_val()
                print '  ',attrconfig.attrname,ident_abs
        
    def has_attrname(self,attrname):
        # TODO: very inefficient, create hash table for attribute names
        #ans = False
        #for attrconfig in self._attrconfigs:
        #    ans = ans | (attrconfig.attrname == attrname)
        #return ans
        
        # attention this is a trick, exploiting the fact that the
        # attribute object with all the attr info is an attribute
        # of the attr manager (=self) 
        return hasattr(self,attrname)
    
    def is_modified(self):
        for attrconf in self._attrconfigs:
            if attrconf.is_modified(): return True
        return False
    
    def get_modified(self):
        modified = []
        for attrconf in self._attrconfigs:
            if attrconf.is_modified(): 
                modified.append(attrconf)
        return modified
    
    def get_configs(self, is_all = False):
        if is_all:
            return self._attrconfigs
        else:
            attrconfigs = []
            for attrconf in self._attrconfigs:
                if len(attrconf.groupnames)>0:
                   if attrconf.groupnames[0]!='_private':
                        attrconfigs.append(attrconf)
                else:
                    attrconfigs.append(attrconf)
                 
            return attrconfigs
        
    def get_obj(self):
        return self._obj
            
    def add(self, attrconf):
        """
        Add a one or several new attributes to be managed.
        kwargs has attribute name as key and Attribute configuration object
        as value.
        """  
        #for attrname, attrconf in attrconfs.iteritems():
        
        attrname = attrconf.attrname
        attrconf.set_obj(self._obj)
        attrconf.set_manager(self)
        
        # set configuration object sa attribute of AttrManager
        setattr(self, attrname, attrconf)
        
        # append also to the list of managed objects
        self._attrconfigs.append(attrconf)
        
        # insert in groups
        if len(attrconf.groupnames) > 0:
            for groupname in attrconf.groupnames:
                
                if not self._groups.has_key(groupname):
                    self._groups[groupname]=[]
                
                self._groups[groupname].append(attrconf)
            
        if self._plugin:
            self._plugin.exec_events(EVTADD, attrconf)
            
        # return default value as attribute of managed object 
        if attrconf.struct == 'scalar':  
            return attrconf.get_init() 
        else:
            return None # table configs do their own init
        


        
    def get_groups(self):
        return self._groups
    
    def get_groupnames(self):
        return self._groups.keys()
    
    def has_group(self, groupname):
        return self._groups.has_key(groupname)
        
    def get_group(self,name): 
        """
        Returns a list with attributes that belong to that group name.
        """
        #print 'get_group self._groups=\n',self._groups.keys()
        return self._groups.get(name,[])

    def get_group_attrs(self,name):
        """
        Returns a dictionary with all attributes of a group. 
        Key is attribute name and value is attribute value. 
        """
        #print 'get_group_attrs', self._groups
        attrs={}
        for attrconf in self._groups[name]:
            attrs[attrconf.attrname]=getattr(self._obj, attrconf.attrname)
        return attrs
    
    def print_attrs(self, show_unit = True, show_parentesis=False):
        print 'Attributes of',self._obj.name,'(ident=%s)'%self._obj.ident
        for attrconf in self.get_configs():
            print '  %s =\t %s'%(attrconf.attrname, attrconf.format_value(show_unit=True))       
           
                
    def save_values(self, state):
        """
        Called by the managed object during save to save the
        attribute values. 
        """
        for attrconfig in self.get_configs():
            attrconfig.save_value(state)
            
    def delete(self, attrname):
        """
        Delete attibite with respective name
        """
        #print '.__delitem__','attrname=',attrname
        
        if hasattr(self,attrname): 
            attrconf = getattr(self,attrname)
            if self._plugin:
                self._plugin.exec_events(EVTDEL, attrconf)
            
            if attrconf in self._attrconfigs:
                for groupname in attrconf.groupnames:
                        self._groups[groupname].remove(attrconf)
                        
                self._attrconfigs.remove(attrconf)
                del self.__dict__[attrname]
                del self._obj.__dict__[attrname]
                return True
            return False # attribute not managed
        return False # attribute not existant
     
    def  __getstate__(self):
        #print '__getstate__',self.ident
        #print '  self.__dict__=\n',self.__dict__.keys()
        
        state={} 
        for attr in self.__dict__.keys():
                if attr =='plugin':
                    plugin = self.__dict__[attr]
                    if plugin!=None:
                        state[attr] = True
                    else:
                        state[attr] = False
                    
                elif attr not in ATTRS_NOSAVE:
                    state[attr] = self.__dict__[attr]
                    
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        self._plugins={}
        
        for attr in state.keys():
            #print '  set state',key
            if attr=='plugin':
                if state[attr]==True:
                    self.__dict__[attr] = Plugin(self)
                else:
                    self.__dict__[attr]= None
            else:
                self.__dict__[attr]=state[attr]




class BasicManagerMixin:
    """
    Basic management methods to be inherited by all other managers.
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None, managertype = 'basic'):
        self.managertype = managertype
        self.ident = ident                
        if name == None:
            self.name = ident
        else:
            self.name = name
        
        #self._is_modified = False
        
        self.reset_parent(parent)
        self._is_saved = False
    
   
        
    #def set_modified(self, is_modified):
    #    print '**set_modified',self.ident
    #    if is_modified & (not self._is_modified):
    #        self._is_modified = is_modified
    #        if (self.parent!=None)&is_modified:
    #            self.parent.set_modified(True)
    #    else:
    #        self._is_modified = is_modified
    def __repr__(self):
        #return '|'+self.name+'|'
        return self.format_ident()
        
    def is_modified(self):
        return self.attrman.is_modified()
        
    def get_name(self):
        return self.name

    def get_ident(self):
        return self.ident
    
    def format_ident(self):
        return str(self.ident)
    
    def get_root(self):
        if self.parent != None:
            return self.parent.get_root()
        else:
            return self
        
    def get_ident_abs(self):
        """
        Returns absolute identity.
        This is the ident of this object in the global tree of objects.
        If there is a parent objecty it must also be managed by the 
        object manager.
        """ 
        #print 'obj.get_ident_abs',self.ident,self.parent, type(self.parent)
        if self.parent != None:
            return self.parent.get_ident_abs()+[self.ident]
        else:
            return [self.get_ident()]
        
    def format_ident_abs(self):
        s = ''
        #print 'format_ident_abs',self.get_ident_abs()
        for ident in self.get_ident_abs():
            s += str(ident)+'.'
        return s[:-1]
    
    def get_parent(self):
        return self.parent
        
    def reset_parent(self, parent):
        self.parent=parent
    
     
    def set_attrman(self, attrman):
        # for quicker acces and because it is only on
        # the attribute management is public and also directly accessible
        #setattr(self, attrname,AttrsManager(self))# attribute management
        self.attrman = attrman
        return attrman
        
    def get_attrman(self):
        return self.attrman
    
    
            
                    
    def  __getstate__(self):
        print '__getstate__',self.ident,self._is_saved
        #print '  self.__dict__=\n',self.__dict__.keys()
        state={}
        if not self._is_saved:
            
            # save standart values
            for attr in ATTRS_SAVE:
                if hasattr(self,attr):
                    state[attr]=getattr(self,attr) 
            
            # save all scalar stuctured attributes
            # attrman knows which and how
            self.attrman.save_values(state)
            
            # save also attrman
            state['attrman'] = self.attrman
            self._is_saved = True
            
        else:
            print 'WARNING: object %s already saved'%self.ident    
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        self._plugins={}
        
        for key in state.keys():
            #print '  set state',key
            self.__dict__[key]=state[key]
        
        
        # done in init2_config...
        # set default values for all states tha have not been saved
        #for attr in self._config.keys():
        #    if (not self._config[attr]['save']) & (not hasattr(self,attr)):
        #        print '  config attr',attr
        #        self.config(attr,**self._config[attr])
            
        # set other states       
        #self._setstate(state)
        
    def init_internal(self):
        """
        Called after set state.
        Link internal states.
        """
        self.attrman.init_internal(self)
        self._is_saved = False
       
    def  init_external(self):
        """
        Called after set state.
        Link internal states.
        """
        self.attrman.init_external()
        
         
class MultitabManagerMixin:
    """
    TODO...
    Multiple table management methods.
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None):
        self.managertype = 'multitab'
        self.ident = ident                
        if name == None:
            self.name = ident
        else:
            self.name = name
        
        #self._is_modified = False
        
    
    
        self._tablemans = []
        self.reset_parent(parent)
        
    #def set_modified(self, is_modified):
    #    print '**set_modified',self.ident
    #    if is_modified & (not self._is_modified):
    #        self._is_modified = is_modified
    #        if (self.parent!=None)&is_modified:
    #            self.parent.set_modified(True)
    #    else:
    #        self._is_modified = is_modified
        
    def is_modified(self):
        return self._is_modified
        
    def get_name(self):
        return self.name

    def get_ident(self):
        return self.ident
    
    def format_ident(self):
        return str(self.ident)
    
    def get_root(self):
        if self.parent != None:
            return self.parent.get_root()
        else:
            return self
        
    def get_ident_abs(self):
        """
        Returns absolute identity.
        This is the ident of this object in the global tree of objects.
        If there is a parent objecty it must also be managed by the 
        object manager.
        """ 
        #print 'obj.get_ident_abs',self.ident,self.parent, type(self.parent)
        if self.parent != None:
            return self.parent.get_ident_abs()+[self.ident]
        else:
            return [self.get_ident()]
        
    def format_ident_abs(self):
        s = ''
        #print 'format_ident_abs',self.get_ident_abs()
        for ident in self.get_ident_abs():
            s += str(ident)+'.'
        return s[:-1]
    
    def get_parent(self):
        return self.parent
        
    def reset_parent(self, parent):
        self.parent=parent

    def set_attrman(self, attrman):
        # for quicker acces and because it is only on
        # the attribute management is public and also directly accessible
        #setattr(self, attrname,AttrsManager(self))# attribute management
        self._attrmanname = attrman.attrname
        return attrman
        
    def get_attrman(self):
        return getattr(self, self._attrmanname)
    
    def add_tableman(self, tableman):
        self._tablemans.append(tableman)
        return tableman
        
    def get_tablemans(self):
        return self._tablemans
         
class ArrayData:
    """
    Empty container used as default to store column data of tables
    """
    def __init__(self, table=None):
        self._table = table
        

class TableEntry:
    def __init__(self, table, id):
        self.table = table
        self.id = id
    
    def __getattr__(self, attrname):
        def method(*args,**kwargs):
                #print("tried to handle unknown method " + attrname)
                getattr(self.table,attrname)(self.id,*args,**kwargs)
        return method 
    
        
class TableManager(AttrsManager,BasicManagerMixin):
    """
    This class is for managing a table within a class.
    In this case specified indexable attributes are columns
    and their entries are the rows.
    
    An element of the table can be called with:
        obj.attrman.attr[id]
    where attr id the attribute's name and id is the row id.
    """
    def __init__(   self,  ident='table', parent = None, name = None, 
                    obj=None, is_keyindex= False, **kwargs ):
        """
        Table manager allows to define arrays and dictionaries
        the will be treated as columns of one and the same table.
        Table column data are kept as attribute with column names of 
        in obj instance.
        """
        
        
        if obj==None:
            obj = ArrayData(self)
            self._is_coldata = True
            
        else:
            self._is_coldata = False
            #BasicManagerMixin._init_objman(self, ident=obj.ident, parent=obj.parent, name= obj.name)
        
        # these makes it a full object with attribute management
        # and itself as trhe only table
        BasicManagerMixin._init_objman(self, ident=ident, parent=parent, name= name)
        self.managertype = 'table'
        self.attrman = self.set_attrman(AttrsManager(self,'attrman'))
        self.add_tableman(self)
        
        # publicly accessible column instance
        # where attributes corrispond to table columns
        # columns are either numpy arrays, lists or dictionaries      
        # (this is the same as the self._obj)
        self.cols = obj # 
        
        
        
        AttrsManager.__init__(self, obj,'_attrs',**kwargs )


        self._is_keyindex = is_keyindex
        
        #print 'check config'#,self.ident,self._config
        self._arrayconfigs=[]
        self.__dictconfigs=[]
        self.__listconfigs=[]
        self._inds = np.zeros((0,),int)
        self._ids = np.zeros((0,),int)
        #self._marrayconfig={}
        
        # TODO create an extra class
        if self._is_keyindex:
            self._key_to_id = {}# Dictionary mapping key to id
            self._id_to_key = {} # Dictionary mapping id to key
        else:
            self._key_to_id = None
            self._id_to_key = None
        # save these attributes, apart from configured ones
        # self._attrconfigs_save+=[ '_arrayconfigs','__dictconfigs','_inds','_ids',
        #                    '_marrayconfig','_is_keyindex','_key_to_id']
    
    #def exportXml(self,f,xmlname=None):
    #    f.write(xm.start(self.ident))
    #    for key in self.get_keys():
    #        f.write(xm.begin('od'))
    #        orig,dest = key
    #        f.write(xm.num('orig',orig))
    #        f.write(xm.num('dest',dest))
    #        f.write(xm.num('dem',self._n_trips[key]))
    #        f.write(xm.end('od'))
    #    f.write(xm.end('ods'))
    def __repr__(self):
        #return '|'+self.name+'|'
        return self.name
    
    def exec_events_keys(self,trigger, attrconf, keys):
        """
        Executes all functions assigned for this trigger.
        Keys are ids if is_keyindex=False otherwise they are index keys (any object)
        """
        if self._has_events:
            #print '**TableManager.exec_events_keys NOMOD',attrconf.attrname,self.ident
            for function in self._events.get(trigger,[]): function(attrconf, keys)
             
    def get_obj(self):
        if self._is_coldata:
            return self
        else:
            return self._obj
       
    def get_ident_abs(self):
        """
        Returns absolute identity.
        This is the ident of this object in the global tree of objects.
        If there is a parent objecty it must also be managed by the 
        object manager.
        """ 
        if self._is_coldata:
            # table has its own column data and an ident of its own right
            return BasicManagerMixin.get_ident_abs(self)
        else:
            # table mimics the ident of the object containing the column data
            return self._obj.get_ident_abs()
     
    def get_name(self):
        if self._is_coldata:
            return self.name
        else:
            return self._obj.get_name()

    def get_ident(self):
        if self._is_coldata:
            return self.ident
        else:
            return self._obj.get_ident()
        
    def add(self, attrconf):
        """
        Add a one or several new attributes to be managed.
        kwargs has attribute name as key and Attribute configuration object
        as value.
        """  
        
        AttrsManager.add(self, attrconf) # insert in common attrs database
        #if self.get_ident_abs() == "gen5test.net.1/1.0":
        #    print 'add',attrconf.attrname,attrconf.struct,self._is_coldata
        
        attrname = attrconf.attrname
        
        if attrconf.struct == 'array':
            # init empty zero array of correct type
            self._arrayconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
        elif attrconf.struct == 'dict':
            # init empty dictionary
            self.__dictconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
        elif attrconf.struct == 'list':
            self.__listconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
        
        elif attrconf.struct == 'scalar':
            val_init = attrconf.get_init()
        
        
        # set initial value if table contains its own arraydata
        #print 'Tableman %s: set attr %s to %s'%(self.name,attrconf.attrname,self._obj)
        #print '  with value',val_init
        #print '  DIR=',self._obj
        #if self._is_coldata: 
        #    setattr(self._obj, attrconf.attrname, val_init)
        #    #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
            
        
        # Return  initial array         
        return val_init
            
   
    
    def is_keyindex(self):
        """
        Returns true if array has key indexed rows
        """    
        return self._is_keyindex
    
    def get_id_from_key(self,key):
        """
        Returns id that corresponds to key.
        """
        return self._key_to_id[key]
    
    def get_key_from_id(self,id):
        """
        Returns key that corresponds to id.
        """
        return self._id_to_key[id]
    
    
    
    def get_ids_from_keys(self,keys):
        ids = [0]*len(keys)
        i=0
        for key in keys:
            ids[i]=self._key_to_id[key]
            i+=1
        return ids   
    
    def get_ind_from_key(self,key):
        return self.get_ind(self.get_id_from_key(key))
    
    def get_inds_from_keys(self,keys):
        return self.get_inds(self.get_ids_from_keys(keys))
        
    def get_keys_from_ids(self,ids):
        keys = ['']*len(ids)
        i=0
        for id in ids:
            keys[i]=self._id_to_key[id]
            i+=1
        return keys 
        # return self._id_to_key[ids] # TODO:no because it is a dict at the moment :(
    
    def get_keys(self,is_ordered = False):
        """
        Returns list of all keyindex .
        """
        #print 'get_keys',self.ident,self._key_to_id.keys()
        if is_ordered:
            return np.array(self.get_keys_from_ids(self._ids),object)
        else:
            return np.array(self._key_to_id.keys())# faste, but random orders
    
    #def get(self,attr,name):
    #    """
    #    Returns value corresponding to is_keyindex id
    #    """
    #    return getattr(self._obj, self.attrname)[self.get_id_from_key(name)]
        
    def get_ids(self,inds = None, is_copy=False):
        """
        Return all ids corrisponding to array indexes inds.
        Options:
            if no inds are given, all ids are returnd
            
            if ordered is true, ids will be sorted before they are returned.
            
        """
        if inds == None:
            if is_copy: 
                return self._ids.copy()
            else:
                return self._ids
        else:
            return self._ids[inds]
    
    def get_id(self,ind):
        """
        Returns scalar id corresponding to index ind
        """
        #print 'get_id',ind,self._ids
        return self._ids[ind]
    
    def select_ids(self,mask):
        """
        Returns an array of ids, corresponding to the True
        of the mask array.
        
        Usage:
            Select all ids for which array of attribute x is greater than zero.
            ids=obj.select_ids( obj.x>0 )
        """
        #print 'select_ids',mask,flatnonzero(mask)
        #print '  self._ids=',self._ids
        
        return np.take(self._ids,np.flatnonzero(mask))
    
    def get_inds(self,ids=None):
        if ids!=None:
            return self._inds[ids]
        else:
            return self._inds[self._ids]
    
    def get_ind(self,id):
        return self._inds[id]
    
    
    def suggest_id(self,is_zeroid=False):
        """
        Returns a an availlable id.
        
        Options:
            is_zeroid=True allows id to be zero.
            
        """
        return self.suggest_ids(1,is_zeroid)[0]
        
    def suggest_ids(self,n,is_zeroid=False):
        """
        Returns a list of n availlable ids.
        It returns even a list for n=1. 
        
        Options:
            is_zeroid=True allows id to be zero.
        """
        # TODO: does always return 1 if is_index is True ?????
        #print 'suggest_ids',n,is_zeroid,self._inds,len(self._inds),self._inds.dtype
        ids_unused_orig = np.flatnonzero(np.less(self._inds,0))
        
        if not is_zeroid:   
            if len(self._inds)==0:
                ids_unused=np.zeros(0,int)
            else:
                # avoid 0 as id:
                #ids_unused=take(ids_unused,flatnonzero(greater(ids_unused,0)))
                #print '  ids_unused_orig',ids_unused_orig,type(ids_unused_orig)
                #print '  len(ids_unused_orig)',len(ids_unused_orig),ids_unused_orig.shape
                #print '  greater(ids_unused_orig,0)',greater(ids_unused_orig,0)
                #print '  len(greater(ids_unused_orig,0))',len(greater(ids_unused_orig,0))
                #print '  flatnonzero(greater(ids_unused_orig,0))',flatnonzero(greater(ids_unused_orig,0))
                #print '  len(flatnonzero(greater(ids_unused_orig,0)))=',len(flatnonzero(greater(ids_unused_orig,0)) )
                ids_unused=ids_unused_orig[np.flatnonzero(np.greater(ids_unused_orig,0))]
            zid=1
        else:
            if len(self._inds)==0:
                ids_unused=np.zeros(0,int) 
            else:
                ids_unused=ids_unused_orig.copy()
            
            zid=0
               
        n_unused=len(ids_unused)
        n_max=len(self._inds)-1
        #print '  ids_unused',ids_unused
        #print '  ids_unused.shape',ids_unused.shape
        #print '  len(ids_unused)',len(ids_unused)
        #print '  n_unused,n_max,zid=',n_unused,n_max,zid
        
        
        
        if n_max<zid:
            # first id generation
            ids=np.arange(zid,n+zid) 
            
        elif n_unused > 0:
            if n_unused >= n:
                ids=ids_unused[:n]
            else:
                #print '  ids_unused',ids_unused 
                #print '  from to',n_max+1,n_max+1+n-n_unused
                #print '  arange=',arange(n_max+1,n_max+1+n-n_unused)
                #print '  type(ids_unused)',type(ids_unused)
                #print '  dtype(ids_unused)',ids_unused.dtype
                ids=np.concatenate((ids_unused,np.arange(n_max+1,n_max+1+n-n_unused))) 
        
        else:
            ids=np.arange(n_max+1,n_max+1+n)
 
        return ids
    
    def suggest_key(self):
        id0 = self.suggest_id(is_zeroid=True)
        n=1
        keys = self.get_keys()
        while str(id0+n+1) in keys:
            n += 1
        
        return str(id0+n+1)
    
    #def export_rowfunctions(self):# use group rowfunction
    #    # return [self.attrman.add_myrow,]
    #    return []
    
    def add_row(self, id = None, key = None):
        
        if id == None:
            return self.add_rows(keys = [key])[0]
        else:
            return self.add_rows(ids = [id])[0]
        
    def add_rows_keyrecycle(self, keys=[]):
        """
        Like add-rows but creates only row with new ids for
        keys that do not exist.
        Returns the complted ids for each key in keys. 
        """
        
        
        ids = np.zeros(len(keys),int)
        inds_new = []
        keys_new =[]
        i = 0
        
        for key in keys:
            if not self.contains_key(key):
                inds_new.append(i)
                keys_new.append(key)
            else:
                ids[i] = self.get_id_from_key(key)
            i+=1
            
        ids_new = self.suggest_ids(len(inds_new))
        self.add_rows(ids_new,keys_new) 
        ids[inds_new]=ids_new
        return ids
                
    def add_rows(self, ids = None, keys = None):
        """
        Creates for each id in ids list an entry for each attribute with 
        array or dict  structure.
        
        If ids is a scalar integer, entries will be generated just for
        this id. 
        
        For is_keyindex objects the list _id_to_key_ is necessary 
        with a key correisponding to each id.  
        """
        #print '\nadd_rows',self.format_ident_abs(),ids,keys
        if keys != None:
            if ids == None:
                n = len(keys)
                ids = self.suggest_ids(n,is_zeroid=True)
            # if each id is supposed to have a _id_to_key 
            # arguments must provide a list with keys for each id
            i=0
            for id in ids:
                #print '  id,keys[i]',id,keys[i],self._key_to_id
                self._key_to_id[keys[i]]=id
                #self.set_attr[id] = 
                self._id_to_key[id] = keys[i]
                i+=1
            #print 'add_rows ids=', ids
            #print '  _id_to_key=',self._id_to_key   
        
        
        
        # no ids to create
        n=len(ids)
        if n==0: return np.zeros(0,int)
                
        id_max=max(ids)
        id_max_old = len(self._inds)-1
        n_array_old = len(self)
        
        ids_existing = np.take(  ids,np.flatnonzero( np.less(ids,id_max_old) )  )
        #print '  ids',ids,'id_max_old',id_max_old,'ids_existing',ids_existing
        
        # check here if ids are still available
        if np.sometrue(  np.not_equal( np.take(self._inds, ids_existing), -1)  ):
            print 'WARNING in create_ids: some ids already in use',ids_existing
            return np.zeros(0,int)
        
        
        
        # extend index map with -1 as necessary
        if id_max > id_max_old:
            #print 'ext',-1*ones(id_max-id_max_old) 
            self._inds = np.concatenate((self._inds, -1*np.ones(id_max-id_max_old,int)))
               
        # assign n new indexes to new ids
        ind_new = np.arange(n_array_old,n_array_old+n)
        
        #print 'ind_new',ind_new
        np.put(self._inds,ids,ind_new)
        
        #print '  concat ids..',self._ids,ids
        self._ids = np.concatenate((self._ids,ids))
        
        
            
        # Extend all arrays with n values from args or default        
        for attrconf in self._arrayconfigs:
            #print '  attr',attrconf.name
            attrconf.add(ids, keys)
            
           
                
        
        # assign defaults to all dicts
        for attrconf in self.__dictconfigs:
            #if attrconf.attrname != '_id_to_key':
            attrconf.add(ids, keys)
            
            #i = 0
            # element by element assignment
            #for id in ids:
            #    #print '  dict attr=',attr,value
            #    dic[id] = defaults[i]
            #    i += 1
        
        # assign defaults to all dicts
        for attrconf in self.__listconfigs:
            attrconf.add(ids, keys)
            
        
        return ids
    
    def get_row(self,key):
        if self._is_keyindex:
            id = self.get_id_from_key(key)
            #attrconf.exec_events_item(EVTADDITEM,keys=[key])
        else:
            id = key
            #attrconf.exec_events_item(EVTADDITEM,ids=[id])
        attrs={}    
        for attrconfig in self._attrconfigs:
            if attrconfig.struct in ('array','dict','list'):
                attrs[attrconfig.attrname] = attrconfig[id]
        
        return attrs
    
    def get_rowobj(self, id ):
        """
        Returns an instance of Rowobject of row id
        """  
        if self._is_coldata:
            return RowObj(self, id, self.ident)
        else:
            return RowObj(self, id, self._obj.ident)
              
                    
    def set_row(self,key,**attrs):
        """
        Sets and creates a row if not existant:
            for keyindex key is the key
            for id index key is id
        Attention returns always the id
        """
        if self._is_keyindex:
            if not self.contains_key(key):
                self.add_row(key = key)
            id = self.get_id_from_key(key)
            
        else:
            id = key
            if id not in self:
                self.add_row(id = id)
            
            
        for attrname,value in attrs.iteritems():
            # overwrite only existant attributes
            if hasattr(self,attrname):
                getattr(self,attrname)[id] = value
        
        
        return id
        
    def del_row(self, key):
        if self._is_keyindex:
            id = self.get_id_from_key(key)
        else:
            id = key
        del self[id]
     
    def del_rows(self, keys):
        for key in keys:
            self.del_row(key)
    
    def print_attrs(self, show_unit = True, show_parentesis=False):
        print 'Attributes of Table',self.get_name(),'(ident=%s)'%self.get_ident()
        for attrconf in self._attrconfigs:
            #print '\n %s %s'%(attrconf.name,attrconf.format_unit(show_parentesis = True))
            #print 'attrconf.name,attrconf.groupnames',attrconf.name,attrconf.groupnames
            if attrconf.groupnames[0] != '_private':
                for id in self.get_ids():
                    if self._is_keyindex:
                        print '  %s[%s] =\t %s'%(attrconf.attrname,self.get_key_from_id(id),attrconf.format_value(id,show_unit=False))
                    else:
                        #print '             **',attrconf.attrname,id,'*',attrconf.format_value(id,show_unit=False),'*'
                        print '  %s[%d] =\t %s'%(attrconf.attrname,id,attrconf.format_value(id,show_unit=False))   
                       
                   
    def export_csv(self, filepath=None, sep=',', name_id='ID', 
                    file=None, attrconfigs = None, ids = None, groupname = None,
                    is_header = True, is_ident = False, is_timestamp = True):
        
        
        if filepath!=None:
            file=open(filepath,'w')
            
        if ids == None:
            ids = self.get_ids()
            
        if groupname !=None:
            attrconfigs = self.get_group(groupname)
            is_exportall = False
        if attrconfigs == None:
            attrconfigs = self._attrconfigs
            is_exportall = False
        else:
            is_exportall = True
        
        # header
        if is_header:
            
            row = self._clean_csv(self.get_name(),sep)
            if is_ident:
                row+= sep+'(ident=%s)'%self.format_ident_abs()
            file.write(row+'\n')
            if is_timestamp:
                now = datetime.now()
                file.write(self._clean_csv(now.isoformat(),sep)+'\n')
            file.write('\n\n')
        
        # first table row
        row = name_id
        for attrconf in attrconfigs:
            
            if len(attrconf.groupnames)>0:
                is_private = attrconf.groupnames[0] == '_private'
            else:
                is_private = False
                
            if ((not is_private)&(attrconf.is_save()))|is_exportall:
                row +=sep+self._clean_csv(self.format_symbol(attrconf),sep)
        file.write(row+'\n')
        
        # rest
        for id in ids:
            if self._is_keyindex:
                row = str(self.get_key_from_id(id))#.__repr__()
            else:
                row = str(id)
            row = self._clean_csv(row,sep)
            for attrconf in attrconfigs:
                if len(attrconf.groupnames)>0:
                    is_private = attrconf.groupnames[0] == '_private'
                else:
                    is_private = False
                if ((not is_private)&(attrconf.is_save()))|is_exportall:
                    row+= sep+self._clean_csv('%s'%(attrconf.format_value(id,show_unit=False)),sep)
            
            # make sure there is no CR in the row!!
            #print  row
            file.write(row+'\n')    
        
        if filepath!=None:
            file.close()
    
    def _clean_csv(self,row,sep):
        row=row.replace('\n',' ')
        #row=row.replace('\b',' ')
        row=row.replace('\r',' ')
        #row=row.replace('\f',' ')
        #row=row.replace('\newline',' ')
        row=row.replace(sep,' ')
        return row
    
    def format_symbol(self, attrconf):
        if hasattr(attrconf,'symbol'):
            symbol = attrconf.symbol
        else:
            symbol = attrconf.name
        unit=attrconf.format_unit(show_parentesis=True)
        symbol+=' '+unit  
        return symbol
                  
    def print_rows(self, show_unit = True, show_parentesis=False):
        
        print 'Rows of Table',self.get_name(),'(ident=%s)'%self.format_ident_abs()
        for id in self.get_ids():
            if self._is_keyindex:
                print '\n ID=%s'%self.get_key_from_id(id)
            else:
                print '\n ID=%d'%id
            for attrconf in self._attrconfigs:
                if attrconf.groupnames[0] != '_private':
                    print '  %s =\t %s'%(attrconf.attrname,attrconf.format_value(id,show_unit=True))
       
            
    def __contains__(self,id):
        if (id<len(self._inds))&(id>=0):
            return self._inds[id]>-1
        else:
            return False
        
    def contains_key(self,key):
        return self._key_to_id.has_key(key)
    
    def has_key(self,key):
        return self._key_to_id.has_key(key)
    
    def contains(self,key):
        if self._is_keyindex:
            return self._key_to_id.has_key(key) 
        else:
            return key in self   
        
    def __len__(self):
        """
        Determine current array length (same for all arrays)
        """
        
        return len(self._ids)
    
    def delete(self,attr):
        """
        Delete an attribite from table management and managed object
        """
        #if AttrsManager.__delitem__(attr):
        
        
        
        attrconf = getattr(self, attr)
        #attrconf.exec_events(EVTDEL)
        if attrconf in self._attrconfigs:
            
            # delete attribute and its configuration
            if attrconf.struct == 'array':
                self._arrayconfigs.remove(attrconf)
            
            elif attrconf.struct=='dict':
                self.__dictconfigs.remove(attrconf)
                
            elif attrconf.struct=='list':
                self.__listconfigs.remove(attrconf)
        
        if AttrsManager.delete(self,attr):
            # basic removal successful
            return True
        else:
            print 'WARNING could not remove attr "%s" from Table ident "%s"'%(attr,self.ident)
            return False

    #def _del_item(self, ids):
    #    """
    #    Class specific delete operations 
    #    """
    #    pass
        
    def __delitem__(self,ids):
        """
        remove rows correspondent to the given ids from all array and dict
        attributes
        """
        if not hasattr(ids, '__iter__'):
            ids = [ids]
        
        # note clean: decision: make it before calling del[ids]
        #self._del_item(ids)
        
        
            
        for id in ids:
            if self._is_keyindex:
                key = self._id_to_key[id]
            else:
                key = None
            #print '    removed',key,id
            
            #print '  start deleting id',id
            
            #print '      self._ids',self._ids
            #print '      self._inds',self._inds
            
            #i=self.get_index(id) 
            i=self._inds[id]
            
            #print '     ind=',i
            #print '     del rows from array-types...'
            
            # TODO: this could be done in with array methods
            for attrconf in self._arrayconfigs:
                #print '      del',attr,id,i 
                attrconf.delete(id)
                
                
                
            
            
            
            #print '    del from dicts'
            for attrconf in self.__dictconfigs:  
                #print '      del',attr,id
                attrconf.delete(id)
                
            for attrconf in self.__listconfigs:  
                #print '      del',attr,id
                attrconf.delete(id)
            
            if self._is_keyindex:
                #print '  Remove is_keyindex'
                key=self._id_to_key[id]
                del self._key_to_id[key]
                del self._id_to_key[id]
                
            #print '    del from id lookup'
            self._ids=np.concatenate((self._ids[:i],self._ids[i+1:]))
            
            #print '    free index',id
            if id == len(self._inds)-1:
                # id is highest, let's shrink index array by 1
                self._inds=self._inds[:-1]
            else:
                self._inds[id]=-1   
                    
            # get ids of all indexes which are above i
            ids_above=np.flatnonzero(self._inds>i)
                
            # decrease index from those wich are above the deleted one
            #put(self._inds, ids_above,take(self._inds,ids_above)-1)
            self._inds[ids_above]-=1
            
            #print '    self._inds',self._inds
            
        
                
                    
        #print '  del',ids,' done.'
    
 

    
class ObjManagerMixin(BasicManagerMixin):
    """
    Manages attributes of an object 
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None, info ='',
                        is_tab = True):

        BasicManagerMixin._init_objman(self, ident=ident, parent=parent, name= name)
        self.info = info
        if is_tab:
            self._tablemans  = [] # list of table managements
    
        return True
    
    
        

def save_obj(obj,filename, is_not_save_parent=False):
    """
    Saves python object to a file with filename.
    Filename may also include absolute or relative path.
    If operation fails a False is returned and True otherwise.
    """
    try:
        file=open(filename,'wb')
    except:
        print 'WARNING in save: could not open',simname
        return False
	
	if is_not_save_parent:
	    parent = obj.parent
	    obj.parent = None
    
    pickle.dump(obj, file, protocol=2)
    file.close()
    
    if is_not_save_parent:
        obj.parent=parent
    return True

def load_obj(filename,parent=None):
    """
    Reads python object from a file with filename and returns object.
    Filename may also include absolute or relative path.
    If operation fails a None object is returned.
    """
    print 'load_obj',filename
    try:
        file=open(filename,'rb')
    except:
        print 'WARNING in load_obj: could not open',filename
        return None
    
    #try:
    #print '  pickle.load...'
    obj=pickle.load(file)
    file.close()
    #print '  obj._link2'
    
    # _init2_ is to restore INTERNAL states from INTERNAL states
    obj.init_internal()
    
    # _init3_ is to restore INTERNAL states from EXTERNAL states 
    # such as linking
    obj.init_external()
    
    # _init4_ is to do misc stuff when everything is set
    #obj._init4_config()
    
    return obj

 
class EmptyClass(ObjManagerMixin):
    """
    This claass is empty and used as default.
    
    """
    
    def __init__(self, ident = 'empty',  parent = None, name = 'Empty Object'):
        self._init_objman(ident, parent = parent, name = name)
        self.attrman = self.set_attrman(AttrsManager(self))

        
                        
class TestClass(ObjManagerMixin):
        def __init__(self, ident = 'testobj',  parent=None, name = 'Test Object'):
            self._init_objman(ident, parent=parent, name = name)
            self.attrman = self.set_attrman(AttrsManager(self))
            
            
            
            
            self.access = self.attrman.add(AttrConf(  'access', ['bus','bike','tram'],
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Access list', 
                                        info = 'List with vehicle classes that have access',
                                        ))
                                        
            self.emissiontype = self.attrman.add(AttrConf(  'emissiontype', 'Euro 0',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Emission type', 
                                        info = 'Emission type of vehicle',
                                        ))
                                        
            self.x = self.attrman.add(AttrConf(  'x', 0.0,
                                        groupnames = ['state'], 
                                        perm='r', 
                                        is_save = True,
                                        unit = 'm',
                                        metatype = 'length', 
                                        name = 'position', 
                                        info = 'Test object position',
                                        ))
                                        
            self.is_pos_ok = self.attrman.add(AttrConf(  'is_pos_ok', False,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Pos OK', 
                                        info = 'True if position is OK',
                                        ))
                                        
            
                                            


class TestClass2(ObjManagerMixin):
        def __init__(self,ident = 'testobj2',  parent=None, name = 'Test Object2'):
            self._init_objman(ident,parent=parent, name = name)
            self.attrman = self.set_attrman(AttrsManager(self))
            
            self.child1 = self.attrman.add(  ChildConf( TestClass('child1',self) )
                                            )           

class TestClass3(ObjManagerMixin):
        def __init__(self,ident = 'testobj3',  parent=None, name = 'Test Object3'):
            self._init_objman(ident = ident,  parent=parent, name = name)
            self.attrman = self.set_attrman(AttrsManager(self))
            self.child2 = self.attrman.add(  LinkConf( TestClass('child1',self) ))
            

                                  
###############################################################################
if __name__ == '__main__':
    """
    Test
    """
    
    def on_event_delattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'will be deleted!!'                            
    
    def on_event_setattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'has been set to a new value',attrconfig.format()
    
    def on_event_getattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'has been retrieved the value',attrconfig.format()
    
    def on_event_additem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':added keys:',keys
    
    def on_event_delitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':delete keys:',keys
    
    def on_event_setitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':set keys:',keys
    
    def on_event_getitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':get keys:',keys
    
    
    obj2 = TestClass2()
    obj2.attrman.print_attrs()
    obj2.child1.attrman.x.set(1.0)
    save_obj(obj2,'test_obj2.obj')
    del obj2
    obj2_new = load_obj('test_obj2.obj')
    obj2_new.attrman.print_attrs()
    
    obj2_new.child1.attrman.print_attrs()
    
    sys.exit()
    
    obj = TestClass()
    print 'obj.ident',obj.ident
    
    print 'This is the value of the attribute: obj.x=',obj.x
    #print 'This is the configuration instance of the attribute x',obj.attrman.x
    print 'obj.attrman.x.format():',obj.attrman.x.format()
    #obj.attrman.x.add_event(EVTSET,on_event_setattr)
    #obj.attrman.x.add_event(EVTGET,on_event_getattr)
    #print 'obj.attrman.get_groups()',obj.attrman.get_groups()
    #print 'obj.tab.get_groups()',obj.tab.get_groups()
    
    #print 'Test func...',obj.attrman.testfunc.get()
    #obj.attrman.testfunc.add_event(EVTGET,on_event_getattr)
    #obj.attrman.testfunc.get()
    print 'obj.attrman.x.get()',obj.attrman.x.get(),'is_modified',obj.is_modified()
    obj.attrman.x.set(1.0)
    print 'obj.attrman.x.get()',obj.attrman.x.get(),'is_modified',obj.is_modified()
    
    #obj.attrman.delete('x')
    obj.attrman.print_attrs()
    save_obj(obj,'test_obj.obj')
    del obj
    obj_new = load_obj('test_obj.obj')
    obj_new.attrman.print_attrs()
    #print 'obj.attrman.x.get_formatted()=',obj.attrman.x.get_formatted()
    #print 'obj.x',obj.x
    
                                                           