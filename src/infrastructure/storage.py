import json
import os
from ..interfaces.hardware import IStorage

class FileStorage(IStorage):
    def __init__(self, filepath: str = "blind_data.json"):
        self.filepath = filepath

    def save_position(self, position: int):
        data = {}
        if os.path.exists(self.filepath):
            try:
                with open(self.filepath, 'r') as f:
                    data = json.load(f)
            except json.JSONDecodeError:
                pass

        data['position'] = position
        with open(self.filepath, 'w') as f:
            json.dump(data, f)

    def load_position(self) -> int:
        if not os.path.exists(self.filepath):
            return -1

        try:
            with open(self.filepath, 'r') as f:
                data = json.load(f)
                return data.get('position', -1)
        except json.JSONDecodeError:
            return -1
