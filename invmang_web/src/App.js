import React, { useEffect, useState } from 'react';

function App() {
  const [objects, setObjects] = useState([]);
  const [relationships, setRelationships] = useState([]);

  useEffect(() => {
    fetch('/api/objects')
      .then(response => response.json())
      .then(data => setObjects(data.objects))
      .catch(error => console.error('Error fetching objects:', error));
  }, []);

  useEffect(() => {
    fetch('/api/relationships')
      .then(response => response.json())
      .then(data => setRelationships(data.relationships))
      .catch(error => console.error('Error fetching relationships:', error));
  }, []);

  return (
  <div>
      <h1>Inventory Management</h1>
      <h2>Objects</h2>
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

      <h2>Relationships</h2>
      <table>
        <thead>
          <tr>
            <th>Parent ID</th>
            <th>Child ID</th>
          </tr>
        </thead>
        <tbody>
          {relationships.map((ship, index) => (
            <tr key={`${ship.parent_id}-${ship.child_id}-${index}`}>
              <td>{ship.parent_id}</td>
              <td>{ship.child_id}</td>
            </tr>
          ))}
        </tbody>
      </table>
      {/* Add forms and buttons for adding and moving objects */}
    </div>
  );
}

export default App;
