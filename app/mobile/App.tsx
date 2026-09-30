import React, { useState } from 'react';
import {
  SafeAreaView,
  StyleSheet,
  Text,
  View,
  Pressable,
  ScrollView,
} from 'react-native';

type StationStatus = 'available' | 'occupied' | 'maintenance';

type Station = {
  id: number;
  name: string;
  status: StationStatus;
};

const initialStations: Station[] = [
  {
    id: 1,
    name: 'Polia 01',
    status: 'available',
  },
  {
    id: 2,
    name: 'Polia 02',
    status: 'occupied',
  },
  {
    id: 3,
    name: 'Polia 03',
    status: 'maintenance',
  },
];

export default function App() {
  const [stations, setStations] = useState<Station[]>(initialStations);

  function toggleMaintenance(stationId: number) {
    setStations((currentStations) =>
      currentStations.map((station) => {
        if (station.id !== stationId) {
          return station;
        }

        return {
          ...station,
          status:
            station.status === 'maintenance'
              ? 'available'
              : 'maintenance',
        };
      }),
    );
  }

  function getStatusText(status: StationStatus) {
    switch (status) {
      case 'available':
        return 'Disponível';

      case 'occupied':
        return 'Ocupada';

      case 'maintenance':
        return 'Manutenção';
    }
  }

  function getStatusColor(status: StationStatus) {
    switch (status) {
      case 'available':
        return '#16a34a';

      case 'occupied':
        return '#dc2626';

      case 'maintenance':
        return '#2563eb';
    }
  }

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.content}>
        <View style={styles.header}>
          <Text style={styles.title}>Gym Pole Monitor</Text>

          <Text style={styles.subtitle}>
            Monitoramento das estações de polia
          </Text>
        </View>

        <Text style={styles.sectionTitle}>Estações</Text>

        {stations.map((station) => (
          <View key={station.id} style={styles.card}>
            <View style={styles.cardHeader}>
              <Text style={styles.stationName}>
                {station.name}
              </Text>

              <View
                style={[
                  styles.statusIndicator,
                  {
                    backgroundColor: getStatusColor(
                      station.status,
                    ),
                  },
                ]}
              />
            </View>

            <Text
              style={[
                styles.statusText,
                {
                  color: getStatusColor(station.status),
                },
              ]}
            >
              {getStatusText(station.status)}
            </Text>

            {station.status !== 'occupied' && (
              <Pressable
                style={[
                  styles.button,
                  station.status === 'maintenance' &&
                    styles.releaseButton,
                ]}
                onPress={() => toggleMaintenance(station.id)}
              >
                <Text style={styles.buttonText}>
                  {station.status === 'maintenance'
                    ? 'Liberar estação'
                    : 'Colocar em manutenção'}
                </Text>
              </Pressable>
            )}
          </View>
        ))}
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f3f4f6',
  },

  content: {
    padding: 20,
  },

  header: {
    marginBottom: 30,
  },

  title: {
    fontSize: 28,
    fontWeight: '700',
    color: '#111827',
  },

  subtitle: {
    marginTop: 6,
    fontSize: 15,
    color: '#6b7280',
  },

  sectionTitle: {
    fontSize: 22,
    fontWeight: '700',
    marginBottom: 15,
    color: '#111827',
  },

  card: {
    backgroundColor: '#ffffff',
    borderRadius: 12,
    padding: 18,
    marginBottom: 15,
    elevation: 3,
  },

  cardHeader: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
  },

  stationName: {
    fontSize: 20,
    fontWeight: '600',
    color: '#111827',
  },

  statusIndicator: {
    width: 14,
    height: 14,
    borderRadius: 7,
  },

  statusText: {
    marginTop: 10,
    fontSize: 16,
    fontWeight: '600',
  },

  button: {
    marginTop: 18,
    backgroundColor: '#2563eb',
    paddingVertical: 12,
    borderRadius: 8,
    alignItems: 'center',
  },

  releaseButton: {
    backgroundColor: '#16a34a',
  },

  buttonText: {
    color: '#ffffff',
    fontSize: 15,
    fontWeight: '600',
  },
});