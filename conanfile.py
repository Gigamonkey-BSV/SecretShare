from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from os import environ

class SecretShareConan (ConanFile):
    name = "SecretShare"
    version = "1.1"
    license = "MIT"
    author = "Daniel Krawisz"
    url = "https://github.com/Gigamonkey-BSV/SecretShare"
    description = "Shamir's Secret Shareing program"
    topics = ()
    settings = "os", "compiler", "build_type", "arch"   
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = "src/*"
    requires = "gigamonkey/v0.0.15@proofofwork/stable"

    def config_options (self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build (self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package (self):
        self.copy("SecretShare", dst="bin", keep_path=False)
        

