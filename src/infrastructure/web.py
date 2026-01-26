from fastapi import FastAPI, Request, BackgroundTasks
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pydantic import BaseModel
from ..application.controller import BlindController
import subprocess
import os
import sys

app = FastAPI()
templates = Jinja2Templates(directory="templates")

controller: BlindController = None # Dependency injection via main

class MoveCommand(BaseModel):
    cmd: str

class GotoCommand(BaseModel):
    percent: int

@app.get("/", response_class=HTMLResponse)
async def read_root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})

@app.get("/health")
async def health():
    return {"status": "ok"}

@app.get("/state")
async def get_state():
    return controller.get_state_dict()

@app.post("/move")
async def move(cmd: MoveCommand):
    if cmd.cmd == "up":
        controller.move_up()
    elif cmd.cmd == "down":
        controller.move_down()
    elif cmd.cmd == "stop":
        controller.stop_blind()
    return controller.get_state_dict()

@app.post("/goto")
async def goto(cmd: GotoCommand):
    controller.move_to(cmd.percent)
    return controller.get_state_dict()

@app.post("/home")
async def home():
    controller.home()
    return controller.get_state_dict()

def run_update_script():
    """Runs the update script in the background."""
    # Assume script is at root/scripts/update.py
    # We are in src/infrastructure/web.py
    # Root is ../../../
    root_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    script_path = os.path.join(root_dir, "scripts", "update.py")
    log_path = os.path.join(root_dir, "update.log")

    # Use the same python interpreter
    cmd = [sys.executable, script_path]

    # Ensure log file exists or creates it
    with open(log_path, "a") as log_file:
        subprocess.Popen(cmd, stdout=log_file, stderr=subprocess.STDOUT, cwd=root_dir)

@app.post("/system/update")
async def system_update(background_tasks: BackgroundTasks):
    background_tasks.add_task(run_update_script)
    return {"status": "Update process started. Check logs."}

def create_app(blind_controller: BlindController) -> FastAPI:
    global controller
    controller = blind_controller
    return app
