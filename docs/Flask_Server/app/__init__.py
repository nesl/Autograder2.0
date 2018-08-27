from flask import Flask
from config import Config
from flask_login import LoginManager
from flask_pymongo import PyMongo
from flask_mail import Mail



app = Flask(__name__)
app.config.from_object(Config) #Applies configuration to Flask app
login = LoginManager(app) 
login.login_view = 'login' #Takes user to login page instead of index if not logged in
mail = Mail(app) #Mail object for emailing users
mongo = PyMongo(app) #make PyMongo object for database

#Must be down here to prevent circular dependency issues
from app import routes, models, errors



