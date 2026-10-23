#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <limits>
#include <cstdlib> 

using namespace std;

// CÓDIGOS DE COLOR ANSI Y UTILIDADES

#define RESET   "\033[0m"
#define NEGRITA "\033[1m"
#define ROJO    "\033[31m"
#define VERDE   "\033[32m"
#define AMARILLO "\033[33m"
#define AZUL    "\033[34m"
#define MAGENTA "\033[35m"
#define CIAN    "\033[36m"
#define BLANCO  "\033[37m"


// ANIMACIONES DE CONSOLA

void animarCarga(string mensaje, int iteraciones) {
    cout << mensaje;
    for (int i = 0; i < iteraciones; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(150)); // Un poco más rápido para no desesperar
    }
    cout << endl;
}

void animarBarraProceso() {
    cout << CIAN << "[SISTEMA] Procesando validaciones de pasarela: [" << RESET;
    for (int i = 0; i < 15; ++i) {
        cout << VERDE << "#" << RESET;
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cout << CIAN << "] 100% OK\n" << RESET << endl;
}

// Efecto maquina de escribir
void escribirPausado(string texto, int velocidad_ms = 20) {
    for (char c : texto) {
        cout << c;
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(velocidad_ms));
    }
    cout << endl;
}

// Pausa para leer resultados
void pausarPantalla() {
    cout << "\nPresione Enter para continuar...";
    cin.get();
}

// =================================================================================
// 1. ENUMERACIONES Y CONSTANTES 
// =================================================================================
enum TipoPantalla { IPS, AMOLED, OLED };
enum TipoComprobante { BOLETA, FACTURA };
enum Rol { CAJERO, ADMINISTRADOR };

string pantallaToString(TipoPantalla tp) {
    if (tp == IPS) return "IPS LCD";
    if (tp == AMOLED) return "AMOLED";
    return "OLED";
}

string comprobanteToString(TipoComprobante tc) {
    return (tc == BOLETA) ? "BOLETA DE VENTA" : "FACTURA COMERCIAL";
}

string rolToString(Rol r) {
    return (r == CAJERO) ? "CAJERO" : "ADMINISTRADOR";
}

int solicitarEntero(string mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << ROJO << "[!] Error: Ingrese un numero valido.\n" << RESET;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
    }
}

// =================================================================================
// 2. MÓDULO DE ACTORES Y PERSONAS
// =================================================================================
class Persona {
protected:
    string dni;
    string nombre;
    string telefono;
    string correo;
public:
    Persona(string _dni, string _nombre, string _telefono, string _correo)
        : dni(_dni), nombre(_nombre), telefono(_telefono), correo(_correo) {}
    virtual ~Persona() {}
    string getNombre() { return nombre; }
    string getDni() { return dni; }
};

class Cliente : public Persona {
private:
    string historial_compra;
    double dineroDisponible;
public:
    Cliente(string _dni, string _nombre, string _telefono, string _correo, string _historial, double _dinero)
        : Persona(_dni, _nombre, _telefono, _correo), historial_compra(_historial), dineroDisponible(_dinero) {}

    double getDinero() { return dineroDisponible; }
    void descontarDinero(double monto) { dineroDisponible -= monto; }
};

class Venta;

class Trabajador : public Persona {
private:
    Rol puesto;
    vector<double> pagos;
public:
    Trabajador(string _dni, string _nombre, string _telefono, string _correo, Rol _puesto)
        : Persona(_dni, _nombre, _telefono, _correo), puesto(_puesto) {}

    Rol getPuesto() { return puesto; }

    void recibirPago(double monto) {
        pagos.push_back(monto);
        cout << VERDE << "[OK] " << RESET << "El trabajador " << nombre << " recibio un pago de S/. " << monto << endl;
    }

    void procesarVenta(Venta* venta);
    void gestionarInventario() {
        animarCarga("[LOGISTICA] Sincronizando existencias con Almacen-Atico", 3);
        cout << VERDE << "[OK] " << RESET << "Modificacion de stock consolidada.\n" << endl;
    }
};

// =================================================================================
// 3. MÓDULO DE PRODUCTOS E INVENTARIO
// =================================================================================
class Producto {
protected:
    int id;
    string marca;
    double costo;
    double ganancia;
    bool isAvailable;
public:
    Producto(int _id, string _marca, double _costo, double _ganancia, bool _isAvailable)
        : id(_id), marca(_marca), costo(_costo), ganancia(_ganancia), isAvailable(_isAvailable) {}
    virtual ~Producto() {}

    int getId() { return id; }
    double getPrecioVenta() { return costo + ganancia; }
    string getMarca() { return marca; }
    virtual void mostrarFicha() = 0;
};

class Celular : public Producto {
private:
    string nombreCelular;
    string ram;
    string rom;
    string microprocesador;
    string bateria;
    int anioFabricacion;
    TipoPantalla tipoPantalla;
    string resolucionCamara;
    string sims;
public:
    Celular(int _id, string _marca, double _costo, double _ganancia, bool _isAvailable,
            string _nombre, string _ram, string _rom, string _cpu, string _bat, int _anio, 
            TipoPantalla _pantalla, string _camara, string _sims)
        : Producto(_id, _marca, _costo, _ganancia, _isAvailable),
          nombreCelular(_nombre), ram(_ram), rom(_rom), microprocesador(_cpu), 
          bateria(_bat), anioFabricacion(_anio), tipoPantalla(_pantalla), 
          resolucionCamara(_camara), sims(_sims) {}

    void mostrarFicha() override {
        cout << NEGRITA << "Celular: " << marca << " " << nombreCelular << RESET << "\n"
             << "  [Ficha Tecnica: CPU: " << microprocesador << " | RAM: " << ram << " | ROM: " << rom 
             << " | Bat: " << bateria << " | Pantalla: " << pantallaToString(tipoPantalla) 
             << "\n   Camara: " << resolucionCamara << " | SIMs: " << sims << " | Anio: " << anioFabricacion << "]\n"
             << VERDE << "  Precio de Venta: S/. " << getPrecioVenta() << RESET << "\n" << endl;
    }
};

class Accesorios : public Producto {
protected:
    string descripcion;
public:
    Accesorios(int _id, string _marca, double _costo, double _ganancia, bool _isAvailable, string _desc)
        : Producto(_id, _marca, _costo, _ganancia, _isAvailable), descripcion(_desc) {}
    virtual ~Accesorios() {}
};

class Funda : public Accesorios {
public:
    Funda(int _id, string _marca, double _costo, double _ganancia, bool _isAvailable, string _desc)
        : Accesorios(_id, _marca, _costo, _ganancia, _isAvailable, _desc) {}

    void mostrarFicha() override {
        cout << NEGRITA << "Funda: " << marca << RESET << " (" << descripcion << ") - Precio: S/. " << getPrecioVenta() << "\n" << endl;
    }
};

class ProtectorPantalla : public Accesorios {
public:
    ProtectorPantalla(int _id, string _marca, double _costo, double _ganancia, bool _isAvailable, string _desc)
        : Accesorios(_id, _marca, _costo, _ganancia, _isAvailable, _desc) {}

    void mostrarFicha() override {
        cout << NEGRITA << "Protector: " << marca << RESET << " (" << descripcion << ") - Precio: S/. " << getPrecioVenta() << "\n" << endl;
    }
};

// =================================================================================
// 4. LOGÍSTICA Y SERVICIOS
// =================================================================================
class Garantia {
private:
    int id;
    int duracion;
    string cobertura;
public:
    Garantia(int _id, int _duracion, string _cobertura)
        : id(_id), duracion(_duracion), cobertura(_cobertura) {}
};

class Entrega {
protected:
    string tipo_entrega;
    string fecha_entrega;
public:
    Entrega(string _tipo, string _fecha) : tipo_entrega(_tipo), fecha_entrega(_fecha) {}
    virtual ~Entrega() {}
};

class EntregaDomicilio : public Entrega {
private:
    string direccionEnvio;
public:
    EntregaDomicilio(string _fecha, string _dir) : Entrega("Delivery / Domicilio", _fecha), direccionEnvio(_dir) {}
};

class EntregaLocal : public Entrega {
private:
    string tiendaRetiro;
public:
    EntregaLocal(string _fecha, string _tienda) : Entrega("Retiro en Local", _fecha), tiendaRetiro(_tienda) {}
};

// =================================================================================
// 5. MÓDULO TRANSACCIONAL
// =================================================================================
class LineaVenta {
private:
    int id;
    Producto* producto;
    int cantidad;
    double precioUnitario;
    Garantia* garantia;
public:
    LineaVenta(int _id, Producto* _prod, int _cant, Garantia* _gar)
        : id(_id), producto(_prod), cantidad(_cant), garantia(_gar) {
        precioUnitario = producto->getPrecioVenta();
    }

    double calcularSubtotal() { return precioUnitario * cantidad; }
};

class Venta {
private:
    int id;
    double total;
    TipoComprobante comprobante;
    Cliente* cliente;
    Trabajador* vendedor;
    Entrega* entrega;
    vector<LineaVenta> lineasVenta;
public:
    Venta(int _id, TipoComprobante _comp, Cliente* _cli, Trabajador* _ven, Entrega* _ent)
        : id(_id), total(0.0), comprobante(_comp), cliente(_cli), vendedor(_ven), entrega(_ent) {}

    ~Venta() { lineasVenta.clear(); }

    void agregarLinea(LineaVenta linea) {
        lineasVenta.push_back(linea);
        total += linea.calcularSubtotal();
    }

    double getTotal() { return total; }
    Cliente* getCliente() { return cliente; }
    TipoComprobante getComprobante() { return comprobante; }
};

void Trabajador::procesarVenta(Venta* venta) {
    cout << CIAN << "\n==================================================" << endl;
    cout << "           SISTEMA DE CONTROL DE CAJA             " << endl;
    cout << "==================================================" << RESET << endl;
    cout << "Atendido por: " << NEGRITA << nombre << RESET << " (" << rolToString(puesto) << ")" << endl;
    cout << "Cliente: " << venta->getCliente()->getNombre() << endl;
    cout << "Documento: " << comprobanteToString(venta->getComprobante()) << endl;
    cout << "Monto Bruto: S/. " << venta->getTotal() << endl;
    
    animarBarraProceso();
    
    cout << AMARILLO << "--------------------------------------------------" << endl;
    cout << "       EJECUTANDO FILTRO FINANCIERO OBLIGATORIO   " << endl;
    cout << "--------------------------------------------------" << RESET << endl;
    
    if (venta->getCliente()->getDinero() >= venta->getTotal()) {
        venta->getCliente()->descontarDinero(venta->getTotal());
        animarCarga("[FILTRO FINANCIERO] Fondos aprobados. Grabando transaccion", 3);
        
        cout << "\n" << VERDE << NEGRITA;
        escribirPausado("[OK] TRANSACCION EXITOSA CON COMPROBANTE EMITIDO.", 20);
        cout << RESET;
        
        cout << "Nuevo Saldo del Cliente: S/. " << venta->getCliente()->getDinero() << endl;
        cout << CIAN << "==================================================" << RESET << endl;
        gestionarInventario();
        pausarPantalla();
    } else {
        cout << "\n" << ROJO << NEGRITA;
        escribirPausado("[X] ALERTA - FILTRO FINANCIERO RECHAZADO [RESTRICCION DE SALDO]", 20);
        cout << RESET;
        
        cout << "Motivo: Saldo insuficiente." << endl;
        cout << "Monto Faltante: S/. " << (venta->getTotal() - venta->getCliente()->getDinero()) << endl;
        cout << CIAN << "==================================================" << RESET << endl;
        pausarPantalla();
    }
}

// =================================================================================
// 6. ADMINISTRACIÓN Y FINANZAS
// =================================================================================
class Administracion {
public:
    void gestionarProveedores() {
        cout << AZUL << "\n[ADMIN] Abriendo modulo de Proveedores..." << RESET << endl;
        animarCarga("  Buscando contratos y ordenes de adquisicion activas", 3);
        cout << VERDE << "  [OK] " << RESET << "Proveedores cargados: Samsung Peru, Apple LATAM, Spigen Inc." << endl;
        pausarPantalla();
    }
    void pagarNomina(vector<Trabajador*>& listaTrabajadores) {
        cout << AZUL << "\n[ADMIN] Procesando pago de planilla de trabajadores..." << RESET << endl;
        animarCarga("  Enviando transacciones interbancarias a tesoreria", 3);
        for (auto t : listaTrabajadores) {
            t->recibirPago(1800.00); 
        }
        cout << VERDE << "  [OK] " << RESET << "Planilla de sueldos cerrada exitosamente." << endl;
        pausarPantalla();
    }
    void pagarServicios(string servicio, double monto) {
        cout << AZUL << "\n[ADMIN] Procesando pago de costos fijos..." << RESET << endl;
        animarCarga("  Validando recibo digital de " + servicio, 3);
        cout << VERDE << "  [OK] " << RESET << "Pago aceptado para: " << servicio << " por un monto de S/. " << monto << endl;
        pausarPantalla();
    }
};

// =================================================================================
// PRINCIPAL - MENÚ DE ROLES
// =================================================================================
int main() {
    system("cls");
    vector<Producto*> inventario;
    inventario.push_back(new Celular(1, "Apple", 2200.0, 400.0, true, "iPhone 15", "8GB", "128GB", "A16 Bionic", "3349 mAh", 2023, OLED, "48MP", "eSIM"));
    inventario.push_back(new Celular(2, "Samsung", 1500.0, 300.0, true, "Galaxy S24", "12GB", "256GB", "Exynos 2400", "4000 mAh", 2024, AMOLED, "50MP", "Dual SIM"));
    inventario.push_back(new Celular(3, "Xiaomi", 650.0, 150.0, true, "Redmi Note 13", "6GB", "128GB", "Dimensity 6080", "5000 mAh", 2024, IPS, "108MP", "Dual SIM"));
    inventario.push_back(new Funda(4, "Spigen", 30.0, 15.0, true, "Carcasa Rugged Armor"));
    inventario.push_back(new ProtectorPantalla(5, "Gorilla", 15.0, 10.0, true, "Lamina Vidrio Templado 9H"));

    Cliente* clienteActual = new Cliente("45871256", "Alex Mendoza", "951753624", "alex@lima.com", "Ninguno", 2700.0);

    // PERSONAL: Administrador (índice 0) y 3 Cajeros (índices 1, 2 y 3)
    vector<Trabajador*> personal;
    personal.push_back(new Trabajador("00000001", "Administrador", "999999999", "admin@tienda.com", ADMINISTRADOR));
    personal.push_back(new Trabajador("11111111", "Huanca Choquehuanca Juan Jose", "911111111", "int1@tienda.com", CAJERO));
    personal.push_back(new Trabajador("22222222", "Huallpa Aquino Freddy Franklin", "922222222", "int2@tienda.com", CAJERO));
    personal.push_back(new Trabajador("33333333", "Mendoza Tola joe Aldair", "933333333", "int3@tienda.com", CAJERO));

    Garantia* garantiaComun = new Garantia(1, 12, "Garantia Integral del Fabricante");
    Entrega* tipoEntregaLocal = new EntregaLocal("19/10/2026", "Sede Central");
    Administracion gestorAdmin;
    int rolSeleccionado = 0;

    cout << MAGENTA << NEGRITA;
    escribirPausado(">>> BIENVENIDO AL SISTEMA DE GESTION DE TIENDA <<<", 20);
    cout << RESET;
    animarCarga("[SISTEMA] Iniciando kernel con control de acceso por Roles", 3);
    this_thread::sleep_for(chrono::milliseconds(500));

    do {
        system("cls");
        cout << CIAN << "\n=============================================" << RESET << endl;
        cout << AZUL << NEGRITA << "     CONTROL DE ACCESO: TIENDA CELULARES     " << RESET << endl;
        cout << CIAN << "=============================================" << RESET << endl;
        cout << VERDE << "1. Iniciar sesion como Cliente" << RESET << endl;
        cout << AMARILLO << "2. Iniciar sesion como Cajero" << RESET << endl;
        cout << ROJO << "3. Iniciar sesion como Administrador" << RESET << endl;
        cout << "4. Salir" << endl;
        cout << CIAN << "=============================================" << RESET << endl;
        
        rolSeleccionado = solicitarEntero("Seleccione su rol de ingreso: ");

        if (rolSeleccionado == 1) { // ROL: CLIENTE
            int opcionMenu = 0;
            do {
                system("cls");
                cout << VERDE << "\n--- SUB-MENU: PORTAL CLIENTE ---" << RESET << endl;
                cout << "1. Consultar Perfil y Presupuesto Disponible" << endl;
                cout << "2. Desplegar Catalogo de Productos" << endl;
                cout << "3. Configurar Carrito y Procesar Compra" << endl;
                cout << "4. Volver al menu de Roles" << endl;
                opcionMenu = solicitarEntero("Seleccione una opcion: ");
                
                switch(opcionMenu) {
                    case 1:
                        cout << "\n--- PERFIL DE CLIENTE ---" << endl;
                        cout << "Titular: " << clienteActual->getNombre() << " | Saldo Fijo: S/. " << clienteActual->getDinero() << endl;
                        pausarPantalla();
                        break;
                    case 2:
                        cout << "\n--- PRODUCTOS EN EXISTENCIA ---" << endl;
                        for (auto prod : inventario) {
                            cout << "ID [" << prod->getId() << "] -> ";
                            prod->mostrarFicha();
                        }
                        pausarPantalla();
                        break;
                    case 3: {
                        int compSel = solicitarEntero("Tipo Comprobante (1 para BOLETA, 2 para FACTURA): ");
                        TipoComprobante comprobanteElegido = (compSel == 2) ? FACTURA : BOLETA;
                        
                        // Se asigna la venta al cajero 1 por defecto
                        Venta* nuevaCompra = new Venta(9001, comprobanteElegido, clienteActual, personal[1], tipoEntregaLocal);
                        int prodId = 0, nroLinea = 1;
                        
                        cout << "\n" << CIAN << "Ingrese los IDs de los productos." << RESET << endl;
                        cout << AMARILLO << "IMPORTANTE: Escriba '0' (cero) y pulse Enter para CERRAR EL CARRITO e ir a Caja." << RESET << "\n" << endl;
                        do {
                            prodId = solicitarEntero("\nID del Producto (o escriba 0 para ir a Caja): ");
                            if (prodId > 0 && prodId <= (int)inventario.size()) {
                                int cant = solicitarEntero("Cantidad de unidades: ");
                                if (cant > 0) {
                                    LineaVenta linea(nroLinea++, inventario[prodId - 1], cant, garantiaComun);
                                    nuevaCompra->agregarLinea(linea);
                                    cout << VERDE << "[OK] " << RESET << "Agregado al flujo transaccional." << endl;
                                    cout << AMARILLO << NEGRITA << "AVISO: Para concluir la compra, escriba '0' y pulse Enter" << RESET;
                                    this_thread::sleep_for(chrono::milliseconds(1000)); // Pausa de 1 segundo
                                }
                            } else if (prodId != 0) {
                                cout << ROJO << "[!] Alerta: ID fuera de rango." << RESET << endl;
                            }
                        } while (prodId != 0);
                        
                        // El cajero 1 procesa la venta
                        personal[1]->procesarVenta(nuevaCompra);
                        
                        delete nuevaCompra;
                        nuevaCompra = nullptr;
                        break;
                    }
                    case 4:
                        cout << "[SISTEMA] Saliendo del portal del cliente..." << endl;
                        break;
                    default:
                        cout << ROJO << "[!] Opcion invalida." << RESET << endl;
                }
            } while (opcionMenu != 4);

        } else if (rolSeleccionado == 2) { // ROL: CAJERO
            system("cls");
            cout << AMARILLO << "\n--- SELECCIONE SU PERFIL DE CAJERO ---" << RESET << endl;
            cout << "1. " << personal[1]->getNombre() << endl;
            cout << "2. " << personal[2]->getNombre() << endl;
            cout << "3. " << personal[3]->getNombre() << endl;
            cout << "4. Volver" << endl;
            
            int cajeroSel = solicitarEntero("Seleccione su numero de cajero: ");
            Trabajador* cajeroEnTurno = nullptr;

            if (cajeroSel == 1) cajeroEnTurno = personal[1];
            else if (cajeroSel == 2) cajeroEnTurno = personal[2];
            else if (cajeroSel == 3) cajeroEnTurno = personal[3];
            else if (cajeroSel == 4) continue;

            if (cajeroEnTurno != nullptr) {
                int opcionCajero = 0;
                do {
                    system("cls");
                    cout << AMARILLO << "\n--- PORTAL CAJERO: " << cajeroEnTurno->getNombre() << " ---" << RESET << endl;
                    cout << "1. Ver estado de mi caja" << endl;
                    cout << "2. Ver catalogo de productos (consulta)" << endl;
                    cout << "3. Cerrar turno y volver" << endl;
                    opcionCajero = solicitarEntero("Seleccione una opcion: ");
                    
                    switch(opcionCajero) {
                        case 1:
                            cout << "\n[Caja] Turno de " << cajeroEnTurno->getNombre() << " aperturado." << endl;
                            pausarPantalla();
                            break;
                        case 2:
                            cout << "\n--- CATALOGO (VISTA CAJERO) ---" << endl;
                            for (auto prod : inventario) {
                                cout << "ID [" << prod->getId() << "] -> ";
                                prod->mostrarFicha();
                            }
                            pausarPantalla();
                            break;
                        case 3:
                            cout << "[SISTEMA] Cerrando turno de " << cajeroEnTurno->getNombre() << "..." << endl;
                            break;
                        default:
                            cout << ROJO << "[!] Opcion invalida." << RESET << endl;
                    }
                } while (opcionCajero != 3);
            }

        } else if (rolSeleccionado == 3) { // ROL: ADMINISTRADOR
            int opcionAdmin = 0;
            do {
                system("cls");
                cout << ROJO << "\n--- SUB-MENU: PORTAL ADMINISTRADOR ---" << RESET << endl;
                cout << "1. Ejecutar Gestion de Proveedores" << endl;
                cout << "2. Procesar Pago de Planilla (Nomina)" << endl;
                cout << "3. Ejecutar Pago de Servicios Operativos" << endl;
                cout << "4. Volver al menu de Roles" << endl;
                opcionAdmin = solicitarEntero("Seleccione una operacion de gerencia: ");
                
                switch (opcionAdmin) {
                    case 1:
                        gestorAdmin.gestionarProveedores();
                        break;
                    case 2:
                        gestorAdmin.pagarNomina(personal);
                        break;
                    case 3: {
                        int servSel = solicitarEntero("Servicio a pagar (1 para Luz Comercial, 2 para Internet de Tienda): ");
                        string servNom = (servSel == 2) ? "Internet Fibra" : "Luz del Local";
                        gestorAdmin.pagarServicios(servNom, 450.50);
                        break;
                    }
                    case 4:
                        cout << "[SISTEMA] Cerrando panel administrativo..." << endl;
                        break;
                    default:
                        cout << ROJO << "[!] Opcion invalida." << RESET << endl;
                }
            } while (opcionAdmin != 4);
        }

    } while (rolSeleccionado != 4);

    // LIMPIEZA 
    for (auto prod : inventario) delete prod;
    inventario.clear();
    for (auto pers : personal) delete pers;
    personal.clear();
    
    delete clienteActual;    clienteActual = nullptr;
    delete garantiaComun;    garantiaComun = nullptr;
    delete tipoEntregaLocal; tipoEntregaLocal = nullptr;

    cout << "\n" << CIAN;
    escribirPausado("[SISTEMA] Memoria dinamica liberada. Codigo cerrado al 100%.", 20);
    cout << RESET;

    return 0;
}
