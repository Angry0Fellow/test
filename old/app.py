import os
import mysql.connector
from flask import Flask, render_template

app = Flask(__name__)

db_config = {
    'user': os.environ.get('DB_USER'),
    'password': os.environ.get('DB_PASSWORD'),
    'host': os.environ.get('DB_HOST'),
    'database': os.environ.get('DB_NAME'),
}

def get_db_connection():
    return mysql.connector.connect(**db_config)

@app.route('/')
def index():
    db_connection = get_db_connection()
    cursor = db_connection.cursor(dictionary=True)
    
    # Fetch data from the 'objects' table
    cursor.execute("SELECT * FROM objects")
    objects = cursor.fetchall()
    
    cursor.close()
    db_connection.close()
    return render_template('index.html', objects=objects)

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
