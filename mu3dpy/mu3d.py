import ctypes
import os
from sys import platform

class Graph(object):
    """
    Wrapper for the c++ mu3d::graph object.
    """
    
    def __init__(self):
        """
        Initialises the object to call c++ functions.
        """
        dir_path = os.path.dirname(os.path.realpath(__file__))
        if platform == "linux" or platform == "linux2" or platform == "darwin":
            self.cgraph = ctypes.CDLL(dir_path + "/mu3d.so")
        elif platform == "win32":
            self.cgraph = ctypes.WinDLL(dir_path + "/mu3d.dll")
        else:
            raise Exception("unsupported platform")

        self.cgraph._graph.restype = ctypes.c_void_p
        self.cgraph._load.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.cgraph._unfold.argtypes = [ctypes.c_void_p, ctypes.c_int16, ctypes.c_int16]
        self.cgraph._unfold.restype = ctypes.c_bool
        self.cgraph._save.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
        self.cgraph._save_extra.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        self.cgraph._save_unified.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.obj = self.cgraph._graph()

    def delete(self):
        """
        Deletes the graph object.
        :return: None
        """
        self.cgraph._deleteGraph()

    def load(self, file):
        """
        Loads the given mesh from an *.off file.
        :param file: file containing the mesh
        :return: None
        """
        self.cgraph._load(self.obj, file.encode())

    def unfold(self, maxits, optits):
        """
        Tries to unfold the loaded mesh within <maxits> iterations
        and further optimises the unfolding w.r.t spatial extent for <optits>
        iterations.
        :param maxits: maximum number of iterations until the unfolding terminates
        :param optits: maximum number of iterations until the optimisation terminates
        :return: true if a non-overlapping unfolding was found, otherwise false
        """
        return self.cgraph._unfold(self.obj, maxits, optits)

    def save_single(self, file):
        """
        Saves the current unfolding to an obj formatted file with the given name.
        Only the 2D representation of the unfolding is saved.
        :param file: File the unfolding is saved to.
        :return: None
        """
        self.cgraph._save_unified(self.obj, file.encode())

    def save(self, modelfile, gluetabfile):
        """
        Saves the 3D model to an obj formatted file with the given name,
        where the 2D coordinates (of the unfolding) are saved in the texture coordinates.
        The glue tabs are saved with 2D coordinates in an obj formatted file with the given name.
        :param modelfile: File where the 3D model is saved to.
        :param gluetabfile: File where the unfolded glue tabs are saved to.
        :return: None
        """
        self.cgraph._save(self.obj, modelfile.encode(), gluetabfile.encode())

    def save_all(self, modelfile, gluetabfile, gluetabmirrorfile):
        """
        Saves the 3D model to an obj formatted file with the given name,
        the glue tabs and mirrored glue tabs are saved with 2D coordinates in an obj formatted file with the given name.
        :param modelfile: File where the 3D model is saved to.
        :param gluetabfile: File where the unfolded glue tabs are saved to.
        :param gluetabmirrorfile: File where the mirrored unfolded glue tabs (glue area on faces) are saved to.
        :return: None
        """
        self.cgraph._save_extra(self.obj, modelfile.encode(), gluetabfile.encode(), gluetabmirrorfile.encode())