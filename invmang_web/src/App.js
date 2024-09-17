import React, { useEffect, useState } from 'react';

function App() {
  const [objects, setObjects] = useState([]);

  useEffect(() => {
    fetch('/api/objects')
      .then(response => response.json())
      .then(data => setObjects(data.objects))
      .catch(error => console.error('Error fetching objects:', error));
  }, []);

  return (
    <div>
      <h1>Inventory Management</h1>
      <table>
        <thead>
          <tr>
            <th>ID</th>
            <th>Serial</th>
            <th>Name</th>
          </tr>
        </thead>
        <tbody>
          {objects.map(obj => (
            <tr key={obj.id}>
              <td>{obj.id}</td>
              <td>{obj.serial}</td>
              <td>{obj.name}</td>
            </tr>
          ))}
        </tbody>
      </table>
      {/* Add forms and buttons for adding and moving objects */}
    </div>
  );
}

export default App;
