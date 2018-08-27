#This file contains classes for storing data, such as user accounts

from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import UserMixin
from app import mongo, login, app
from time import time
import jwt


#Class that represents registered user. Inherits from provided Mixin class 
#which gives it a few methods necessary to use the flask_login package.
class Student(UserMixin):

	def __init__(self, fname = 'none', lname = 'none', studentID = '1234', email = 'no_email', password_hash = 'no_password'):
		self.firstName = fname
		self.lastName = lname
		self.id = studentID
		self.email = email
		self.password_hash = generate_password_hash(password_hash)
		self.grade = 'N/A'

	def __repr__(self):
		return '<User {}>'.format(self.name)

	def set_password(self, password):
		self.password_hash = generate_password_hash(password)

	def check_password(self, password):
		return check_password_hash(self.password_hash, password)

	def set_grade(self, grade):
		self.grade = grade 

	#Returns the student object as a json-like dict structure for writing to mongoDB
	def jsonify(self):
		return {
			"firstName": str(self.firstName),
			"lastName": str(self.lastName),
			"id": str(self.id),
			"email": str(self.email),
			"password_hash": str(self.password_hash),
			"grade": str(self.grade)
		}

	#Takes a dict and writes its data into the student object
	def buildFromDict(self, dictionary):
		if dictionary is None:
			return
		self.firstName = dictionary['firstName']
		self.lastName = dictionary['lastName']
		self.id = dictionary['id']
		self.email = dictionary['email']
		self.password_hash = dictionary['password_hash']
		self.grade = dictionary['grade']

	def get_reset_password_token(self, expires_in=600):
		return jwt.encode(
			{'reset_password': self.email, 'exp': time() + expires_in},
			app.config['SECRET_KEY'], algorithm='HS256').decode('utf-8')

	@staticmethod
	def verify_reset_password_token(token):
		try:
			email = jwt.decode(token, app.config['SECRET_KEY'], 
						algorithms=['HS256'])['reset_password']
		except Exception as e:
			print('ERROR: ')
			print(e)
			return
		return mongo.db.students.find_one({'email' : str(email)})


#Loads user when logged in user is navigating website
@login.user_loader
def load_user(id):
    user = mongo.db.students.find_one({ 'id': id }) 
    student = Student()
    student.buildFromDict(user)
    return student

