export interface Field {
  id: number;
  name: string;
  color: string;
  price: number;
  type: string;
}

//todo Reihenfolge anpassen!
export const fields: Field[] = [
  { id: 1, name: 'Start', color: 'yellow', price: 0, type: 'corner' },
  { id: 2, name: 'Chur', color: 'light-blue', price: 2420, type: 'property' },
  { id: 3, name: 'Schafhausen', color: 'light-blue', price: 2000, type: 'property' },
  { id: 4, name: 'Aarau', color: 'light-blue', price: 2000, type: 'property' },
  { id: 5, name: 'Neuenburg', color: 'gray', price: 0, type: 'railroad' },
  { id: 6, name: 'Thun', color: 'dark-purple', price: 1200, type: 'property' },
  { id: 7, name: 'Basel', color: 'dark-purple', price: 1200, type: 'property' },
  { id: 8, name: 'Solothurn', color: 'orange', price: 4000, type: 'property' },
  { id: 9, name: 'Freiburg', color: 'orange', price: 3600, type: 'property' },
  { id: 10, name: 'Biel', color: 'orange', price: 3600, type: 'property' },
  { id: 11, name: 'Lugano', color: 'purple', price: 3200, type: 'property' },
  { id: 12, name: 'Solothurn', color: 'purple', price: 2800, type: 'property' },
  { id: 13, name: 'Basel', color: 'purple', price: 2800, type: 'property' },
  { id: 14, name: 'Winterthur', color: 'red', price: 4400, type: 'property' },
  { id: 15, name: 'St. Gallen', color: 'red', price: 4400, type: 'property' },
  { id: 16, name: 'Bern', color: 'red', price: 4800, type: 'property' },
  { id: 17, name: 'Luzern', color: 'yellow', price: 5200, type: 'property' },
  { id: 18, name: 'Zürich', color: 'yellow', price: 5200, type: 'property' },
  { id: 19, name: 'Lausanne', color: 'yellow', price: 5600, type: 'property' },
  { id: 20, name: 'Genf', color: 'green', price: 6000, type: 'property' },
  { id: 21, name: 'Basel', color: 'green', price: 6000, type: 'property' },
  { id: 22, name: 'Bern', color: 'green', price: 6400, type: 'property' },
  { id: 23, name: 'Lausanne', color: 'dark-blue', price: 7000, type: 'property' },
  { id: 24, name: 'Zürich', color: 'dark-blue', price: 8000, type: 'property' },
  { id: 25, name: 'Gemeinschaftskarten', color: '', price: 0, type: 'community-chest' },
  { id: 26, name: 'Chance', color: '', price: 0, type: 'chance' },
  { id: 27, name: 'Bahnhof Bern', color: 'gray', price: 0, type: 'railroad' },
  { id: 28, name: 'Bahnhof Lausanne', color: 'gray', price: 0, type: 'railroad' },
  { id: 29, name: 'St. Moritz', color: 'red', price: 6000, type: 'property' },
  { id: 30, name: 'Lausanne', color: 'purple', price: 8000, type: 'property' },
  { id: 31, name: 'Zug', color: 'purple', price: 7000, type: 'property' },
  { id: 32, name: 'Zermatt', color: 'orange', price: 4000, type: 'property' },
  { id: 33, name: 'Luzern', color: 'yellow', price: 5200, type: 'property' },
  { id: 34, name: 'Luzern', color: 'green', price: 6000, type: 'property' },
  { id: 35, name: 'Bahnhof Genf', color: 'gray', price: 0, type: 'railroad' },
  { id: 36, name: 'Bahnhof Lugano', color: 'gray', price: 0, type: 'railroad' },
  { id: 37, name: 'Freies Parken', color: '', price: 0, type: 'corner' },
  { id: 38, name: 'Steuern', color: '', price: 0, type: 'fee' },
  { id: 39, name: 'Gehe ins Gefängnis', color: '', price: 0, type: 'corner' },
];
