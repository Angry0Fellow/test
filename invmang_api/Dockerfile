# Use an official GCC image
FROM gcc:latest

# Install necessary packages
RUN apt-get update && \
    apt-get install -y cmake libmysqlcppconn-dev libssl-dev

# Set the working directory
WORKDIR /app

# Copy the current directory contents into the container
COPY . .

# Build the application
RUN mkdir build && cd build && cmake .. && make

# Expose port 18080
EXPOSE 18080

# Run the application
CMD ["./build/invmang_api"]
