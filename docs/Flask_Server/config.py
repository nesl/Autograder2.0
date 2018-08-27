#This file contains configurations for various parts of the website.
#Some information in here is sensitive, so it has been changed to a generic name.
#This information is indicated by an INSERT comment after the line.
#If you are working on this project, contact the admin for the information.
#You can use the information when developing, but be sure to change it back before pushing to github.


import os

class Config(object):
    #Secret key signature for json web tokens
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'secret-key' #INSERT SECRET KEY HERE
    
    #Configuration to access gmail account
    MAIL_SERVER = 'smtp.googlemail.com' 
    MAIL_PORT = 587 
    MAIL_USE_TLS = 1 
    MAIL_USERNAME = 'autograder2.0@gmail.com' 
    MAIL_PASSWORD = 'auto_grader_password' #INSTERT PASSWORD HERE
    ADMINS = ['<admin_email_account>'] #INSERT ADMIN ACCOUNT HERE

    #Configuration to access database
    MONGO_DBNAME = 'test-database37'
    MONGO_URI = 'mongodb://<user>:<pass>@ds119662.mlab.com:19662/test-database37' #INSERT PASSWORD HERE