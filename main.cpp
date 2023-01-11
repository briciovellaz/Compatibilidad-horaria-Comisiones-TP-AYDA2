#include <iostream>
#include <fstream>
#include <list>
#include <tuple>
#include <algorithm>
#include <string>

using namespace std;

typedef tuple<int,float,float,string> Comision;
typedef list<pair<string,list<Comision>>> Materias;
typedef list<pair<string,Comision>> Solucion;
typedef list<Solucion> Soluciones;

bool LeerArchivo(string origen, Materias &materias);
bool son_compatibles(Comision com1,Comision com2);
void horarios_posibles(Materias &materias, Solucion solucion, Soluciones &soluciones);
void mostrarmaterias(Materias materias);
void mostrarSoluciones(Soluciones soluciones);

int main()
{
    Materias materias;
    if (LeerArchivo("Materias.csv",materias)){}else{return 0;};
   // mostrarmaterias(materias);
    Soluciones soluciones;
    Solucion solucionParcial;
    auto itMaterias=materias.begin();
    horarios_posibles(materias,solucionParcial, soluciones);
    mostrarSoluciones(soluciones);
    return 0;
}

void mostrarSoluciones(Soluciones soluciones){  //Muestra las soluciones separadas en opciones. O(nxm) [n soluciones, m cantidad de materias]

    if (soluciones.empty()){
        cout<<"No hay opciones para cursar todas las materias.\n";
        return;
    }
    else {
        int i=1;
        for(auto it=soluciones.begin();it!=soluciones.end();it++){
            cout<<"\nOpcion "<<i<<":\n\n";
            i++;
            for(auto it2=it->begin();it2!=it->end();it2++){
                cout<<"Materia: "<< right<<it2->first<<" - Comision: "<< get<0>(it2->second)<<endl;
            }
        }
    }
}

bool solucion_posible(Solucion solucionParcial){  //Indica si una solucion es posible, viendo si cada comision agregada hasta ese momento es compatible con la ultima agregada. O(n) [n comisiones en la solucionParcial]
    if (solucionParcial.size()>1){
    auto ultimo=--solucionParcial.end();
    for(auto it=solucionParcial.begin();it!=ultimo;it++){
        if (!son_compatibles(it->second,ultimo->second)){
                return false;
            }
        }
    }
    return true;
}

void mostrarmaterias(Materias materias){    // Prueba para ver la carga desde archivo. O(mxc) [m materias, c comisiones de cada materia]
    for(auto itMat=materias.begin();itMat!=materias.end();itMat++){
        cout<<"Materia: "<< itMat->first<<endl;
        for(auto itCom=itMat->second.begin();itCom!=itMat->second.end();itCom++){
                cout<<"Comision "<<get<0>(*itCom)<<endl;
                cout<<"Inicio " << get<1>(*itCom)<<"hs Final "<<get<2>(*itCom)<<"hs Dia " <<get<3>(*itCom)<<endl;

        }
    }
}

bool son_compatibles(Comision com1, Comision com2){  // Chequea si dos comisiones son compatibles (sus horarios no se chocan). O(15)

    float inicio1,inicio2,fin1,fin2;
    string dia1,dia2;
    inicio1=get<1>(com1);
    inicio2=get<1>(com2);
    fin1=get<2>(com1);
    fin2=get<2>(com2);
    dia1=get<3>(com1);
    dia2=get<3>(com2);

    if(inicio1>=inicio2 && inicio1<fin2){ return (dia1!=dia2);} //Verifica que ninguna empiece durante la duracion de la otra. En caso de que asi sea, debe ser en distintos dias.
    if(inicio2>=inicio1 && inicio2<fin1){ return (dia1!=dia2);}

    return true;
}

void horarios_posibles(Materias &materias, Solucion solucion, Soluciones &soluciones){  //Busca las soluciones por fuerza bruta, probando todas las posibilidades. O(C^M) [C el max numero de comisiones que tiene alguna materia y M la cantidad de materias]
    if (materias.empty()){   //Si ya no quedan mas materias para agregar, verifica que la solucion sea valida, de ser asi la agrega a la lista de soluciones finales.
        if(solucion_posible(solucion)){
             soluciones.push_back(solucion);
             return;
        }else {
            return ;   //Si la solucion no es valida, la descarta.
        }
    }
    if (!solucion_posible(solucion)){  //Verifica si la solucion parcial es valida, de no ser asi realiza una poda y no continua agregando comisiones a esta solucion.
        return;
    }
    pair<string,list<Comision>> materia;
    auto it=materias.begin();
    materia=*it;   //Toma una materia para elegir sus comisiones
    it=materias.erase(it);  //Elimina la materia para no volver a tenerla en cuenta

   for(auto itCom=materia.second.begin();itCom!=materia.second.end();itCom++){
        Solucion auxiliar=solucion;
        auxiliar.push_back(make_pair(materia.first,*itCom));  //Agrega un nuevo elemento a la solucion.
        horarios_posibles(materias,auxiliar, soluciones);  //Se invoca recursivamente con la nueva solucion para continuar con la materia siguiente.   Esto se repite con cada comision de cada materia.
    }

    materias.push_front(materia);  //Añade nuevamente las materias.

}

void AgregarComision(Materias &materias, pair<string,Comision> nueva){  //Carga una comision leida del archivo al programa. O(m) [m cant de materias]
    for(auto it=materias.begin();it!=materias.end();it++){
        if (it->first==nueva.first){
            it->second.push_back(nueva.second);
            return;
        }
    }
    list<Comision> listaComisiones;           // si no existia la materia hasta ese entonces, la agrega.
    listaComisiones.push_back(nueva.second);
    materias.push_back(make_pair(nueva.first,listaComisiones));
    return;
}

bool LeerArchivo(string origen, Materias &materias){   // Lee los datos desde un archivo .csv . O(L) [L cantidad de lineas]
    ifstream archivo(origen.c_str());
    if (!archivo.is_open()){
        cout << "No se pudo abrir el archivo: " << origen << endl;
        return false;}
    else {
        string linea;
        //cabecera del csv
        getline(archivo,linea);
        //Leemos de una linea completa por vez (getline).
        while (getline(archivo, linea)){

            int pos_inicial = 0;
            int pos_final = linea.find(',');


            string materia = linea.substr(pos_inicial, pos_final);  //Obtiene el nombre de la materia

            pos_inicial = pos_final + 1;
            pos_final = linea.find(',', pos_inicial);
            string numero=linea.substr(pos_inicial, pos_final - pos_inicial);  // Obtiene el numero de comision
            int nroCom=stoi(numero);

            pos_inicial = pos_final + 1;
            pos_final = linea.find(',', pos_inicial);
            string inicio= linea.substr(pos_inicial, pos_final - pos_inicial);  // Obtiene la hora de inicio
            float in=stof(inicio);

            pos_inicial = pos_final + 1;
            pos_final = linea.find(',', pos_inicial);
            string fin= linea.substr(pos_inicial, pos_final - pos_inicial);  // Obtiene la hora de finalizacion
            float termina=stof(fin);

            pos_inicial = pos_final + 1;
            pos_final = linea.find(',', pos_inicial);
            string dia= linea.substr(pos_inicial, pos_final - pos_inicial);  // Obtiene el dia

            Comision comision=make_tuple(nroCom,in,termina,dia);

            AgregarComision(materias,make_pair(materia,comision)); // O(m)

        }
        return true;
    }
}
