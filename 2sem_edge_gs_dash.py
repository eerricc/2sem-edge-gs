import dash
from dash import dcc, html
from dash.dependencies import Input, Output, State
import plotly.graph_objs as go
import requests

# Constants for IP and port
IP_ADDRESS = ""
PORT_STH = 8666
DASH_HOST = "0.0.0.0"  # Set this to "0.0.0.0" to allow access from any IP

# Function to get luminosity data from the API
def get_luminosity_data(lastN):
    url = f"http://{IP_ADDRESS}:{PORT_STH}/STH/v1/contextEntities/type/Lamp/id/urn:ngsi-ld:Lamp:001/attributes/luminosity?lastN={lastN}"
    headers = {
        'fiware-service': 'smart',
        'fiware-servicepath': '/'
    }
    response = requests.get(url, headers=headers)
    if response.status_code == 200:
        data = response.json()
        try:
            values = data['contextResponses'][0]['contextElement']['attributes'][0]['values']
            return values
        except KeyError as e:
            print(f"Key error: {e}")
            return []
    else:
        print(f"Error accessing {url}: {response.status_code}")
        return []

# Function to get flux data from the API (adjusted from "flow")
def get_flux_data(lastN):
    url = f"http://{IP_ADDRESS}:{PORT_STH}/STH/v1/contextEntities/type/Lamp/id/urn:ngsi-ld:Lamp:001/attributes/flux?lastN={lastN}"
    headers = {
        'fiware-service': 'smart',
        'fiware-servicepath': '/'
    }
    response = requests.get(url, headers=headers)
    if response.status_code == 200:
        data = response.json()
        try:
            values = data['contextResponses'][0]['contextElement']['attributes'][0]['values']
            return values
        except KeyError as e:
            print(f"Key error: {e}")
            return []
    else:
        print(f"Error accessing {url}: {response.status_code}")
        return []

# Set lastN value
lastN = 10  # Get 10 most recent points at each interval

app = dash.Dash(__name__)

app.layout = html.Div([
    html.H1('Sensor Data Viewer'),
    dcc.Graph(id='luminosity-graph'),
    dcc.Graph(id='flux-graph'),  # Adjusted to show "flux" graph
    # Store to hold historical data
    dcc.Store(id='data-store', data={'timestamps': [], 'luminosity_values': [], 'flux_values': []}),
    dcc.Interval(
        id='interval-component',
        interval=10*1000,  # in milliseconds (10 seconds)
        n_intervals=0
    )
])

@app.callback(
    Output('data-store', 'data'),
    Input('interval-component', 'n_intervals'),
    State('data-store', 'data')
)
def update_data_store(n, stored_data):
    # Get luminosity data
    data_luminosity = get_luminosity_data(lastN)
    # Get flux data (adjusted to read "flux")
    data_flux = get_flux_data(lastN)

    if data_luminosity and data_flux:
        # Extract values and timestamps
        luminosity_values = [float(entry['attrValue']) for entry in data_luminosity]
        flux_values = [float(entry['attrValue']) for entry in data_flux]
        timestamps = [entry['recvTime'] for entry in data_luminosity]

        # Append new data to stored data
        stored_data['timestamps'].extend(timestamps)
        stored_data['luminosity_values'].extend(luminosity_values)
        stored_data['flux_values'].extend(flux_values)

        return stored_data

    return stored_data

@app.callback(
    [Output('luminosity-graph', 'figure'),
     Output('flux-graph', 'figure')],  # Adjusted to display flux graph
    Input('data-store', 'data')
)
def update_graphs(stored_data):
    figures = []

    # Luminosity graph
    if stored_data['timestamps'] and stored_data['luminosity_values']:
        mean_luminosity = sum(stored_data['luminosity_values']) / len(stored_data['luminosity_values'])
        trace_luminosity = go.Scatter(
            x=stored_data['timestamps'],
            y=stored_data['luminosity_values'],
            mode='lines+markers',
            name='Luminosity',
            line=dict(color='orange')
        )
        trace_mean_luminosity = go.Scatter(
            x=[stored_data['timestamps'][0], stored_data['timestamps'][-1]],
            y=[mean_luminosity, mean_luminosity],
            mode='lines',
            name='Mean Luminosity',
            line=dict(color='blue', dash='dash')
        )
        fig_luminosity = go.Figure(data=[trace_luminosity, trace_mean_luminosity])
        fig_luminosity.update_layout(
            title='Luminosity Over Time',
            xaxis_title='Timestamp',
            yaxis_title='Luminosity',
            hovermode='closest'
        )
        figures.append(fig_luminosity)
    else:
        figures.append({})

    # Flux graph
    if stored_data['timestamps'] and stored_data['flux_values']:
        mean_flux = sum(stored_data['flux_values']) / len(stored_data['flux_values'])
        trace_flux = go.Scatter(
            x=stored_data['timestamps'],
            y=stored_data['flux_values'],
            mode='lines+markers',
            name='Flux',
            line=dict(color='green')
        )
        trace_mean_flux = go.Scatter(
            x=[stored_data['timestamps'][0], stored_data['timestamps'][-1]],
            y=[mean_flux, mean_flux],
            mode='lines',
            name='Mean Flux',
            line=dict(color='purple', dash='dash')
        )
        fig_flux = go.Figure(data=[trace_flux, trace_mean_flux])
        fig_flux.update_layout(
            title='Flux Over Time',
            xaxis_title='Timestamp',
            yaxis_title='Flux (units)',
            hovermode='closest'
        )
        figures.append(fig_flux)
    else:
        figures.append({})

    return figures

if __name__ == '__main__':
    app.run_server(debug=True, host=DASH_HOST, port=8050)
