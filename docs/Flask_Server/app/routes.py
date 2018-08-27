from flask import render_template, flash, redirect, url_for, request
from flask_login import current_user, login_user, logout_user, login_required, LoginManager
from werkzeug.urls import url_parse
from app import app, mongo #database object
from app.forms import LoginForm, RegistrationForm, ResetPasswordRequestForm, ResetPasswordForm
from app.models import Student
from app.email import send_password_reset_email




studentList = mongo.db.students #This object accesses the student collection on MongoDB


@app.route('/')
@app.route('/index')
@login_required
def index():
    return render_template('index.html', title = 'Home')

#LOGIN AND REGISTRATION
#--------------------------------------------------------------------
@app.route('/login', methods=['GET', 'POST'])
def login():
    if current_user.is_authenticated:
        flash('You are already logged in.')
        return redirect(url_for('index'))
    form = LoginForm()
    if form.validate_on_submit():
        user = studentList.find_one({ 'email' : form.email.data })
        student = Student()
        student.buildFromDict(user)
        if user is None or not student.check_password(form.password.data): #checks that password matches input
            flash('Invalid email or password')
            return redirect(url_for('login'))
        login_user(student, remember = form.remember_me.data)
        next_page = request.args.get('next')
        if not next_page or url_parse(next_page).netloc != '':
            next_page = url_for('index')
        return redirect(next_page)
    return render_template('login.html', title = 'Sign In', form=form)

@app.route('/register', methods = ['GET','POST'])
def register():
    if current_user.is_authenticated:
        flash('Logout before attempting to create a new account.')
        return redirect(url_for('index'))
    form = RegistrationForm()
    if form.validate_on_submit():
        newStudent = Student(form.first_name.data, form.last_name.data,
            form.student_ID.data, form.email.data)
        newStudent.set_password(form.password.data)
        studentList.insert_one(newStudent.jsonify())
        flash('You are now registered. Please login.')
        return redirect(url_for('login'))
    return render_template('register.html', title = 'Register', form = form)
#--------------------------------------------------------------------

@app.route('/webUSB', methods=['GET','POST'])
@login_required
def webUSB():
    return render_template('webUSB.html')

@app.route('/view')
@login_required
def view():
    jsonList = []
    for person in studentList.find():
        jsonList.append(person) 
    return render_template('view.html', students=jsonList)

@app.route('/logout')
@login_required
def logout():
    logout_user()
    flash('Logged out.')
    return redirect(url_for('index'))

@app.route('/user/<email>')
@login_required
def user(email):
    user = studentList.find_one({'email' : email})
    return render_template('user.html', user=user)

#--------------------------------------------------------------------

#PASSWORD RESET ROUTES

@app.route('/reset_password_request', methods=['GET', 'POST'])
def reset_password_request():
    if current_user.is_authenticated:
        flash('You are already logged in.')
        return redirect(url_for('index'))
    form = ResetPasswordRequestForm()
    if form.validate_on_submit():
        user = studentList.find_one({'email' : form.email.data})
        student = Student()
        student.buildFromDict(user)
        if user is not None:
            send_password_reset_email(student)
        flash('Check your email for the instructions to reset your password')
        return redirect(url_for('login'))
    return render_template('reset_password_request.html',title='Reset Password', form=form)

@app.route('/reset_password/<token>', methods=['GET', 'POST'])
def reset_password(token):
    if current_user.is_authenticated:
        flash('You are already logged in. No need to reset your password.')
        return redirect(url_for('index'))
    user = Student.verify_reset_password_token(token)
    if user is None:
        return redirect(url_for('index'))
    form = ResetPasswordForm()
    if form.validate_on_submit():
        student = Student()
        student.buildFromDict(user)
        student.set_password(form.password.data)
        studentList.find_one_and_update(
            {'email' : student.email},
            {'$set' : {'password_hash' : student.password_hash}})
        flash('Your password has been reset')
        return redirect(url_for('login'))
    return render_template('reset_password.html', form=form)

#--------------------------------------------------------------------
