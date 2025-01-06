import React, { useEffect, useState } from 'react';
import { View, Text, StyleSheet, Dimensions, TouchableOpacity } from 'react-native';
import { LineChart } from 'react-native-chart-kit';
import { useRouter } from 'expo-router';

export default function SecondPage() {
  const router = useRouter();
  const [historicalData, setHistoricalData] = useState<HistoricalData>({
    CO2: [],
    CH4: [],
    O2: [],
    CO: [],
  });

  const [timeRange, setTimeRange] = useState('6h'); // Standard-Zeitbereich: 6 Stunden

  type HistoricalData = {
    CO2: HistoricalDataPoint[];
    CH4: HistoricalDataPoint[];
    O2: HistoricalDataPoint[];
    CO: HistoricalDataPoint[];
  };

  type HistoricalDataPoint = {
    timestamp: string; // Zeitstempel als String
    value: number;     // Wert als Zahl
  };

  useEffect(() => {
    const dummyData = generateDummyData(timeRange);

    setHistoricalData(dummyData);
  }, [timeRange]);

  const generateDummyData = (range: string): HistoricalData => {
    // Anzahl der Intervalle basierend auf dem Zeitbereich
    const intervalMinutes = getIntervalMinutes(range); // Intervallgröße in Minuten
    const totalMinutes = getTotalMinutes(range); // Gesamtdauer in Minuten
    const numberOfPoints = 6; // Immer 6 Punkte

    // Dummy-Timestamps erzeugen
    const timestamps: string[] = [];
    const now = new Date();

    for (let i = 0; i < numberOfPoints; i++) {
      const time = new Date(now.getTime() - totalMinutes * 60 * 1000 + i * intervalMinutes * 60 * 1000);
      timestamps.push(`${time.getHours().toString().padStart(2, '0')}:${time.getMinutes().toString().padStart(2, '0')}`);
    }

    const generateValues = (base: number) =>
      timestamps.map((timestamp, index) => ({
        timestamp,
        value: base + index * 2 + Math.random() * 5, // Zufällige Werte
      }));

    return {
      CO2: generateValues(400),
      CH4: generateValues(10),
      O2: generateValues(21),
      CO: generateValues(2),
    };
  };

  const getIntervalMinutes = (range: string): number => {
    switch (range) {
      case '1h':
        return 10; // Alle 10 Minuten
      case '2h':
        return 20; // Alle 20 Minuten
      case '4h':
        return 40; // Alle 40 Minuten
      case '6h':
        return 60; // Alle 60 Minuten (1 Stunde)
      case '12h':
        return 120; // Alle 120 Minuten (2 Stunden)
      default:
        return 60; // Standard: 1 Stunde
    }
  };

  const getTotalMinutes = (range: string): number => {
    switch (range) {
      case '1h':
        return 60; // 1 Stunde
      case '2h':
        return 120; // 2 Stunden
      case '4h':
        return 240; // 4 Stunden
      case '6h':
        return 360; // 6 Stunden
      case '12h':
        return 720; // 12 Stunden
      default:
        return 360; // Standard: 6 Stunden
    }
  };

  const renderChart = (data: HistoricalDataPoint[], label: string) => {
    if (data.length === 0) {
      return <Text style={styles.noDataText}>No Data Available</Text>;
    }

    const timeLabels = data.map((point) => point.timestamp); // Extrahiere Zeitstempel
    const values = data.map((point) => point.value); // Extrahiere Werte

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
              onPress={() => setTimeRange(range)}
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
          <Text style={styles.chartTitle}>O2</Text>
          {renderChart(historicalData.O2, 'O2')}
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
