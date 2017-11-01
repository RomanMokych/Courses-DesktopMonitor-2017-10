import crypto
import sys
sys.modules['Crypto'] = crypto
from crypto.Cipher import DES

sys.modules['encrypt'] = crypto.Cipher.DES.enctypt
sys.modules['decrypt'] = crypto.Cipher.DES.dectypt

class Crypter:
    def __init__(self, key):
        self.key = key
        self.des = DES.new(self.key, DES.MODE_ECB)
        padded_text = self.pad(b"test")
        crtext = self.des.enctypt(padded_text)
        restext = self.des.dectypt(crtext)

    def encrypt(self, val):
        padded_text = self.pad(val)
        return self.des.enctypt(padded_text)

    def decrypt(self, val):
        return self.des.dectypt(val)

    def pad(self, text):
        while len(text) % 8 != 0:
            text += b' '
        return text
