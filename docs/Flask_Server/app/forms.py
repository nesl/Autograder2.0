#This file contains classes for various forms to be called in the routes file

from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField
from wtforms.validators import DataRequired, ValidationError, Email, EqualTo
from app.models import Student
from app import mongo

class LoginForm(FlaskForm):
	email = StringField('email', validators=[DataRequired()])
	password = PasswordField('Password',validators=[DataRequired()])
	remember_me = BooleanField('Remember Me')
	submit = SubmitField('Sign In')

class RegistrationForm(FlaskForm):
	email = StringField('email', validators = [DataRequired(), Email()])
	first_name = StringField('First Name', validators = [DataRequired()])
	last_name = StringField('Last Name', validators = [DataRequired()])
	student_ID = StringField('Student ID', validators = [DataRequired()])
	password = PasswordField('Password', validators = [DataRequired()])
	password2 = PasswordField('Verify Password', validators = [DataRequired(), EqualTo('password')])
	submit = SubmitField('Register')

	#Ensures that the ID number will be 9 digits long and that it has not been used to create an account
	def validate_student_ID(self, student_ID):
		user = mongo.db.students.find_one({'id' : student_ID.data})
		if len(student_ID.data) != 9:
			raise ValidationError('The ID you entered is not valid. It must be 9 characters.')
		elif user is not None:
			raise ValidationError('There is an account already associated with this student ID.')

	#Ensures that the email does not have an account already associated with it
	def validate_email(self, email):
		user = mongo.db.students.find_one({'email' : email.data})
		if user is not None:
			raise ValidationError('There is an account already associated with this email.')

class ResetPasswordRequestForm(FlaskForm):
	email = StringField('Email', validators=[DataRequired(), Email()])
	submit = SubmitField('Request Password Reset')

class ResetPasswordForm(FlaskForm):
	password = PasswordField('Password', validators=[DataRequired()])
	password2 = PasswordField(
		'Repeat Password', validators=[DataRequired(), EqualTo('password')])
	submit = SubmitField('Request Password Reset')