from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pydantic import BaseModel
from ..application.controller import BlindController

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

def create_app(blind_controller: BlindController) -> FastAPI:
    global controller
    controller = blind_controller
    return app
