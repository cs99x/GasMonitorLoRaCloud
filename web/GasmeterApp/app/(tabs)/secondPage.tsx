import React, { useEffect, useState } from 'react';
import { View, Text, StyleSheet, Dimensions, TouchableOpacity } from 'react-native';
import { LineChart } from 'react-native-chart-kit';
import { useRouter } from 'expo-router';
import axios from 'axios'; // Axios für API-Anfragen

export default function SecondPage() {
  const router = useRouter();
  const [historicalData, setHistoricalData] = useState<HistoricalData>({
    CO2: [],
    CH4: [],
    LPG: [],
    CO: [],
  });

  const [timeRange, setTimeRange] = useState('6h'); // Standard-Zeitbereich: 6 Stunden
  const [loading, setLoading] = useState(false); // Zustand, ob die Daten geladen werden

  type HistoricalData = {
    CO2: HistoricalDataPoint[];
    CH4: HistoricalDataPoint[];
    LPG: HistoricalDataPoint[];
    CO: HistoricalDataPoint[];
  };

  type HistoricalDataPoint = {
    timestamp: string; // Zeitstempel als String
    value: number;     // Wert als Zahl
  };

  // Funktion zum Abrufen von Daten vom Backend
  const fetchHistoricalData = async (range: string) => {
    setLoading(true);
    try {
      const response = await axios.get('http://127.0.0.1:5000/data', {
        params: { timeRange: range },
      });
      console.log('API Response:', response.data); // Daten in der Konsole ausgeben
      setHistoricalData(response.data);
    } catch (error) {
      console.error('Error fetching data:', error);
    } finally {
      setLoading(false);
    }
  };  

  // Lade Daten für den Standardzeitbereich beim ersten Rendern
  useEffect(() => {
    fetchHistoricalData(timeRange);
  }, [timeRange]); // Neue Daten laden, wenn sich der Zeitbereich ändert

  const renderChart = (data: HistoricalDataPoint[], label: string) => {
    if (loading) {
      return <Text style={styles.noDataText}>Loading...</Text>;
    }
    
    if (data.length === 0) {
      return <Text style={styles.noDataText}>No Data Available</Text>;
    }
    
    // Daten auf maximal 10 Einträge begrenzen
    const limitedData = data.slice(0, 10);
  
    // Entferne das Datum aus den Zeitstempeln und behalte nur die Uhrzeit
    const timeLabels = limitedData.map((point) => {
      const timeOnly = point.timestamp.split(' ')[1]; // Nimmt nur den Teil nach dem Leerzeichen (die Uhrzeit)
      return timeOnly;
    });
  
    const values = limitedData.map((point) => point.value); // Extrahiere Werte
  
    return (
      <LineChart
        data={{
          labels: timeLabels,
          datasets: [
            {
              data: values,
            },
          ],
        }}
        width={Dimensions.get('window').width * 0.43} // Breite so angepasst, dass 2 Boxen horizontal passen
        height={240} // Höhe des Graphen
        chartConfig={{
          backgroundColor: '#ffffff',
          backgroundGradientFrom: '#ffffff',
          backgroundGradientTo: '#ffffff',
          color: (opacity = 1) => `rgba(0, 128, 128, ${opacity})`,
          labelColor: (opacity = 1) => `rgba(0, 0, 0, ${opacity})`,
          style: {
            borderRadius: 10,
          },
          propsForDots: {
            r: '4',
            strokeWidth: '2',
            stroke: '#4CAF50',
          },
        }}
        style={{
          marginVertical: 8,
          borderRadius: 10,
        }}
      />
    );
  };  
  
  return (
    <View style={styles.container}>
      {/* Header und Buttons */}
      <View style={styles.headerContainer}>
        <Text style={styles.header}>Historical Data</Text>
        <View style={styles.buttonContainer}>
          {['1h', '2h', '4h', '6h', '12h'].map((range) => (
            <TouchableOpacity
              key={range}
              style={[
                styles.button,
                timeRange === range && styles.buttonActive, // Aktiver Button
              ]}
              onPress={() => {
                setTimeRange(range); // Setze den neuen Zeitbereich
              }}
            >
              <Text
                style={[
                  styles.buttonText,
                  timeRange === range && styles.buttonTextActive,
                ]}
              >
                Last {range}
              </Text>
            </TouchableOpacity>
          ))}
        </View>
      </View>

      {/* Vier Boxen mit Graphen, 2 Boxen pro Zeile */}
      <View style={styles.boxContainer}>
        <View style={styles.chartBox}>
          <Text style={styles.chartTitle}>CO2</Text>
          {renderChart(historicalData.CO2, 'CO2')}
        </View>
        <View style={styles.chartBox}>
          <Text style={styles.chartTitle}>CH4</Text>
          {renderChart(historicalData.CH4, 'CH4')}
        </View>
        <View style={styles.chartBox}>
          <Text style={styles.chartTitle}>LPG</Text>
          {renderChart(historicalData.LPG, 'LPG')}
        </View>
        <View style={styles.chartBox}>
          <Text style={styles.chartTitle}>CO</Text>
          {renderChart(historicalData.CO, 'CO')}
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#ffffff',
    padding: 20,
  },
  headerContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 20,
  },
  header: {
    fontSize: 24,
    fontWeight: 'bold',
  },
  buttonContainer: {
    flexDirection: 'row',
    alignItems: 'center',
  },
  button: {
    paddingVertical: 5,
    paddingHorizontal: 10,
    borderWidth: 1,
    borderColor: '#ccc',
    borderRadius: 5,
    marginLeft: 10,
  },
  buttonActive: {
    backgroundColor: '#4CAF50',
    borderColor: '#4CAF50',
  },
  buttonText: {
    fontSize: 14,
    color: '#000',
  },
  buttonTextActive: {
    color: '#fff',
  },
  boxContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    marginTop: 25,
  },
  chartBox: {
    backgroundColor: '#ffffff',
    width: Dimensions.get('window').width * 0.45,
    height: 300,
    borderRadius: 10,
    marginBottom: 20,
    borderWidth: 1,
    borderColor: '#E0E0E0',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 4,
    elevation: 3,
    justifyContent: 'flex-start',
    padding: 10,
  },
  chartTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 10,
  },
  noDataText: {
    fontSize: 14,
    color: 'gray',
  },
});
