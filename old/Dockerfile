# Use the official Python image.
FROM python:3.9-slim

# Set the working directory in the container.
WORKDIR /app

# Copy the requirements file and install dependencies.
COPY requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt

# Copy the application code.
COPY . .

# Set environment variables for database connection.
ENV DB_HOST=db
ENV DB_USER=invuser
ENV DB_PASSWORD=invpassword
ENV DB_NAME=invMang

# Expose port 5000 for the Flask app.
EXPOSE 5000

# Command to run when the container starts.
CMD ["python", "app.py"]
