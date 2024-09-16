import os
import mysql.connector
from flask import Flask, render_template_string

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

    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Inventory Management</title>
        <style>
            table {
                width: 60%%;
                border-collapse: collapse;
                margin: 20px auto;
            }
            th, td {
                border: 1px solid #dddddd;
                text-align: left;
                padding: 8px;
            }
            th {
                background-color: #f2f2f2;
            }
            h2 {
                text-align: center;
            }
        </style>
    </head>
    <body>
        <h2>Product List</h2>
        <table>
            <tr>
                <th>ID</th>
                <th>Name</th>
                <th>Price ($)</th>
            </tr>
            {% for product in products %}
            <tr>
                <td>{{ product['id'] }}</td>
                <td>{{ product['name'] }}</td>
                <td>{{ product['price'] }}</td>
            </tr>
            {% endfor %}
        </table>
    </body>
    </html>
    '''

    return render_template_string(html, products=products)

if __name__ == '__main__':
    app.run(host='0.0.0.0')
