# Base L1 decoder class.
class Decoder:
    def __init__( self, name, cn, tn, mapping ):
        self.name = name
        self.cn = cn
        self.tn = tn
        self.mapping = mapping

    def on_burst( self, burst ):
        raise NotImplementedError( "%s decoder does not implement on_burst." % self.name )
