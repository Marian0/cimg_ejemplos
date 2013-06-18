#include "myCImg.h"

void LoopRecortarAnalizarCanales(CImg<double> img, bool es_hsi) {
    CImg<double> rgb = img;
    std::string title = "Analisis RGB";
    
    if (es_hsi) {
        rgb.HSItoRGB();
        title = "Analisis HSI";

    }
    std::cout<<title.c_str()<<"\n";


	CImgDisplay ventana(rgb, title.c_str());   //Crea una ventana y dibuja la imagen...
    unsigned int CANTIDAD_CLICK = 0;
    //Primer y segundo click
    Pixel click1;
    Pixel click2;

    while ( not ventana.is_closed() && not ventana.is_keyQ()) {
        ventana.wait(); //espera eventos
    
        if (ventana.button() && ventana.mouse_y() >= 0 && ventana.mouse_x() >= 0) { 
            //si se apreta un boton del mouse dentro de la ventana
            if (CANTIDAD_CLICK % 2 == 0){ //primer click
                click1.x = ventana.mouse_x();
                click1.y = ventana.mouse_y();
            } else {    //segundo click
                click2.x = ventana.mouse_x();
                click2.y = ventana.mouse_y();
                std::cout<<"* Click en: ("<<click1.x<<","<<click1.y<<") y ("<<click2.x<<","<<click2.y<<") \n"; 
                
                //Como ya tenemos dos clicks, estamos listos para calcular el histograma
                CImg<double> cropeada = img.get_crop(click1.x, click1.y, click2.x, click2.y);

                CImgList<double> lista;
                if (es_hsi)
                    lista.push_back(cropeada.get_HSItoRGB());
                else
                    lista.push_back(cropeada);

                cimg_forC(cropeada, c) {
                    CImg<double> canal = cropeada.get_channel(c);
                    lista.push_back(canal);

                    std::cout<<"**Canal "<<c<<"\tMinimo: "<<canal.min()<<"\t\t\t Maximo: "<<canal.max()<<"\t\t\tPromedio: "<<canal.mean()<<"\n";
                }
                std::cout<<"\n\n";
                lista.display("Analisis de Colores ", false);

                CImg<double> histograma;
                cimg_forC(cropeada, c) {
                    if (es_hsi && (c == 1 || c==2))
                        histograma = cropeada.get_channel(c).get_histogram(256,0.0,1.0);
                    else
                        histograma = cropeada.get_channel(c).get_histogram(256,0,255);
                    histograma.display_graph(0,3);
                }
            }
            CANTIDAD_CLICK++;
            if (CANTIDAD_CLICK == 2) //acotamos para evitar overflow
                CANTIDAD_CLICK = 0;
        }
    }
}


void LoopCrecimiento(CImg<double> img, double _limite) {
    std::cout<<"\n====ANALISIS DE CRECIMIENTO===="<<"\n";

    CImgDisplay ventana(img, "Analisis de Crecimiento");   //Crea una ventana y dibuja la imagen...
    unsigned int CANTIDAD_CLICK = 0;
    //Primer y segundo click
    Pixel click1;

    while ( not ventana.is_closed() && not ventana.is_keyQ()) {
        ventana.wait(); //espera eventos
    
        if (ventana.button() && ventana.mouse_y() >= 0 && ventana.mouse_x() >= 0) { 
            click1.x = ventana.mouse_x();
            click1.y = ventana.mouse_y();
          
            std::cout<<"* Click en: ("<<click1.x<<","<<click1.y<<") \n"; 

            CImg<bool> crecimiento = SegmentarColorRegion(img, _limite, click1.x, click1.y);

            unsigned int blancos = 0;
            cimg_forXY(crecimiento, x, y) {
                if (crecimiento(x,y) == 1)
                    blancos++;
                
            }
            std::cout<<"\nCantidad de Blancos: "<<blancos<<" Cantidad de Negros "<<crecimiento.size()-blancos<<"\n";
            std::cout<<"*** Con Umbral Limite: "<<_limite<<"\n\n";
            std::cout<<"\n========"<<"\n";

            CImg<double> con_seed = img;
            if (img.spectrum() == 3) {
                con_seed.draw_circle(click1.x, click1.y, 5, Rojo);
                
            } else {
                con_seed.draw_circle(click1.x, click1.y, 5, Gris);

            }


            (con_seed , crecimiento).display("Analizando Semillas", false);
        }
    }
}

Pixel GetNorte(CImg<bool> &  img) {

    unsigned int W = img.width();
    unsigned int H = img.height();

    Pixel p;
    int i,j;
    for ( i = 0; i < H ; ++i ) {
        for ( j = 0; j < W ; ++j ) {
            if ( img(j,i) ) {
                p.x = j;
                p.y = i;
                return p;
            }

        }
    }
    std::cout<<"** Cuidado, imagen negra.\n";
    return p;
}

Pixel GetSur(CImg<bool> &  img) {
    unsigned int W = img.width();
    unsigned int H = img.height();

    Pixel p;
    int i,j;
    for ( i = H-1; i >= 0 ; --i ) {
        for ( j = 0; j < W ; ++j ) {
            if ( img(j,i) ) {
                p.x = j;
                p.y = i;
                return p;
            }

        }
    }
    std::cout<<"** Cuidado, imagen negra.\n";
    return p;

}
Pixel GetEste(CImg<bool> &  img) {
    unsigned int W = img.width();
    unsigned int H = img.height();

    Pixel p;
    int i,j;
    for ( j = W-1; j >= 0 ; --j ) {
        for ( i = H-1; i >= 0 ; --i ) {
            if ( img(j,i) ) {
                p.x = j;
                p.y = i;
                return p;
            }

        }
    }
    std::cout<<"** Cuidado, imagen negra.\n";
    return p;

}
Pixel GetOeste(CImg<bool> &  img) {
    unsigned int W = img.width();
    unsigned int H = img.height();

    Pixel p;
    int i,j;
    for ( j = 0; j <  W ; ++j ) {
        for ( i = H-1; i >= 0 ; --i ) {
            if ( img(j,i) ) {
                p.x = j;
                p.y = i;
                return p;
            }

        }
    }
    std::cout<<"** Cuidado, imagen negra.\n";
    return p;

}
